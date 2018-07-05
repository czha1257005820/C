#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <mntent.h>
#include <string.h>
#include <errno.h>
#include <sys/vfs.h>
#include <dirent.h>

#define MAXBUFSIZE 1024
 
typedef struct
  {
   char* name;
   unsigned long user;
   unsigned long nic;
   unsigned long system;
   unsigned long idle;
  }SysCPUInfo;
 
typedef struct
  {
   unsigned long MemFree;
   unsigned long MemTotal;
  }SysMemInfo;
 
typedef struct
  {
    char type[40];
    char device[40];
    char mntpnt[40];
    unsigned long blocks;
    unsigned long bfree;
    unsigned long availiable_disk;	
    unsigned long bused;
    float bused_percent;
  }DiskInfo;
 
typedef struct
  { 
    unsigned long Disk_Total;
    unsigned long Disk_Free;
  }SysDiskInfo;
 
SysCPUInfo* _GetHostCPUInfo()
{
	SysCPUInfo *cpuinfo = (SysCPUInfo *)malloc(sizeof(SysCPUInfo));	
	FILE 	*fd;
	char	buff[256];
 
	memset(buff, '\0', 256);
	
	fd = fopen("/proc/stat", "r");
	fgets(buff, sizeof(buff), fd);
	
	char name[20];
	unsigned long user;
	unsigned long nic;
	unsigned long system;
	unsigned long idle;
 
	sscanf(buff, "%s %lu %lu %lu %lu", name, &user, &nic, &system, &idle);
	
    cpuinfo->name = name;
	cpuinfo->user = user;
	cpuinfo->nic = nic;
	cpuinfo->system = system;
	cpuinfo->idle = idle;
	//printf("%s %lu %lu %lu %lu\n",cpuinfo->name,cpuinfo->user,cpuinfo->nic,cpuinfo->system,cpuinfo->idle);
 
	fclose(fd);
	return cpuinfo;
}
 
float _CalculateHostCPURate(SysCPUInfo *first, SysCPUInfo *second)
 
{
 
	unsigned long	old_CPU_Time, new_CPU_Time;
	unsigned long	usr_Time_Diff, sys_Time_Diff, nic_Time_Diff;
	float 			cpu_use = 0.0;
 
	old_CPU_Time = (unsigned long)(first->user + first->nic + first->system + first->idle);
	new_CPU_Time = (unsigned long)(second->user + second->nic + second->system + second->idle);
 
	usr_Time_Diff = (unsigned long)(second->user - first->user);
	sys_Time_Diff = (unsigned long)(second->system - first->system);
 
	nic_Time_Diff = (unsigned long)(second->nic -first->nic);
	//printf("old_CPU_Time = %lu, new_CPU_Time = %lu\n",old_CPU_Time,new_CPU_Time);
 
	if ((new_CPU_Time - old_CPU_Time) != 0)
	    cpu_use = (float)100*(usr_Time_Diff + sys_Time_Diff + nic_Time_Diff)/(new_CPU_Time - old_CPU_Time);
 
	else
	  cpu_use = 0.0;

	return cpu_use;
}
  
float GetHostCPURate()
{
	float cpu_rate;
	
	SysCPUInfo *first, *second;
	first = _GetHostCPUInfo();
	sleep(1);
	second = _GetHostCPUInfo();

	cpu_rate = _CalculateHostCPURate(first, second);
 

	free(first); 
	free(second);
	first = second = NULL;
	
	return cpu_rate;
}
 
float GetHostTemRate()
{
    return 0;
}

SysMemInfo* GetHostMemInfo()
{
	SysMemInfo *memInfo = (SysMemInfo *)malloc(sizeof(SysMemInfo));
	
	struct sysinfo tmp;
	int ret = 0;
	
	ret = sysinfo(&tmp);
	if (ret == 0) {
		memInfo->MemFree = (unsigned long)tmp.freeram/(1024*1024);
		memInfo->MemTotal = (unsigned long)tmp.totalram/(1024*1024);
	} 
   
	return memInfo;
}
 
float GetHostMemRate()
{
	float men_rate;
    SysMemInfo* memInfo = GetHostMemInfo();
	
	men_rate = (float)100*(memInfo->MemTotal - memInfo->MemFree)/memInfo->MemTotal;
	
	return men_rate;
}
 
SysDiskInfo* GetHostDiskInfo(char* diskName)
{
   // printf("%s \n",diskName);
	SysDiskInfo *sys_disk_info = (SysDiskInfo *)malloc(sizeof(SysDiskInfo));
	
	DiskInfo		*disk_info;
	struct statfs	fs_info;
	struct mntent	*mnt_info;
	
	FILE			*fh;
	float			percent;
	unsigned long	sum_Total = 0, total_free = 0;
 
	if ((fh = setmntent("/etc/mtab", "r")) == NULL) {
		printf("Cannot open /etc/mtab file!:%s\n",strerror(errno));
		return NULL;
	}
 
	while ((mnt_info = getmntent(fh)) != NULL) {
		if (statfs(mnt_info->mnt_dir, &fs_info) < 0) {
			continue;
		}
		
		if ((disk_info = (DiskInfo *)malloc(sizeof(DiskInfo))) == NULL) {
			continue;
		}
 
		if(strcmp(mnt_info->mnt_fsname, diskName) == 0)
		       {
			if (fs_info.f_blocks != 0) {
 
				percent = (((float)fs_info.f_blocks - (float)fs_info.f_bfree)* 100.0/(float)fs_info.f_blocks);
			       //printf("%f\n",percent); 
			} else {
				percent = 0;
			}
		} else {
			continue;
		}
 
		strcpy(disk_info->type, mnt_info->mnt_type);
		strcpy(disk_info->device, mnt_info->mnt_fsname);
		strcpy(disk_info->mntpnt, mnt_info->mnt_dir);
 
		unsigned long block_size = fs_info.f_bsize/1024;
		disk_info->blocks = fs_info.f_blocks * block_size / 1024;
		disk_info->bfree = fs_info.f_bfree * block_size / 1024;
		disk_info->availiable_disk = fs_info.f_bavail * block_size / 1024;
		disk_info->bused = (fs_info.f_blocks - fs_info.f_bfree) * block_size / 1024;
		disk_info->bused_percent = percent;
 	
 		sum_Total += disk_info->blocks;
		total_free += disk_info->availiable_disk;
 
		free(disk_info);
		disk_info = NULL;
	}
 
	sys_disk_info->Disk_Total = sum_Total/1024;
	sys_disk_info->Disk_Free = total_free/1024;
	return sys_disk_info;
}
 
 float GetHostDiskRate()
{
	float disk_rate;
    SysDiskInfo* diskInfo = GetHostDiskInfo("/dev/sda1");
	
	disk_rate = (float)100*(diskInfo->Disk_Total - diskInfo->Disk_Free)/diskInfo->Disk_Total;
	
	return disk_rate;
}
 
 float GetHostIORate()
 {
    char cmd[] ="iostat -d -x";
    char buffer[MAXBUFSIZE];  
    char a[20];   
    float arr[20];
            
	FILE* pipe = popen(cmd, "r");    
    if (!pipe)  
	    return -1; 
	
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);
    fgets(buffer, sizeof(buffer), pipe);  
	
	sscanf(buffer,"%s %f %f %f %f %f %f %f %f %f %f %f %f %f ",a,&arr[0],&arr[1],&arr[2],&arr[3],&arr[4],&arr[5],&arr[6],&arr[7],&arr[8],&arr[9],&arr[10],&arr[11],&arr[12]);
            //printf("%f\n",arr[12]);
        return arr[12];
    
	pclose(pipe);  
}

int main()
{
    float CpuRate,Temperature,MenRate,DiskRate,IoRate;

    CpuRate = GetHostCPURate();
	Temperature=GetHostTemRate();
	MenRate = GetHostMemRate();
    DiskRate= GetHostDiskRate();
	IoRate  = GetHostIORate();
	
	
	if( CpuRate > 80.0 )
	{
		printf("CPU使用率大于80"); 
	}
	else		
	printf("CpuRate = %f,Temperature = %f, MenRate = %f, DiskRate = %f,IoRate = %f\n"
	,CpuRate,Temperature,MenRate,DiskRate,IoRate);

    return 0;	
}
