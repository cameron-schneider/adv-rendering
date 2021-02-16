/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_app_pdbrm.c
	PDB removal utility.

	**DO NOT MODIFY THIS FILE**

	PDB error: https://stackoverflow.com/questions/28016220/how-do-you-generate-new-pdb-files-every-build-in-visual-studio-2013
	PDB release: https://blog.molecular-matters.com/2017/05/09/deleting-pdb-files-locked-by-visual-studio/
	PDB release example by Summer Softleigh (@xKamuna): https://pastebin.com/Jtwi5Dvz
	PDB release fixes by Alex West & Avery Dibble

	Summer's references: 
	Info on 'File' string https://forum.sysinternals.com/enumerating-file-handles-question_topic23264.html
	Raymon Chen: https://blogs.msdn.microsoft.com/oldnewthing/20120217-00/?p=8283/
	blez https://blez.wordpress.com/2012/09/17/enumerating-opened-handles-from-a-process/
	wj32 https://forum.sysinternals.com/howto-enumerate-handles_topic18892.html
*/

#if (defined _WINDOWS || defined _WIN32)
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>


// for pdb removal
#include <winternl.h>
#include <RestartManager.h>
#pragma comment(lib, "rstrtmgr.lib")

#define STATUS_BUFFER_OVERFLOW			0x80000005
#define STATUS_INFO_LENGTH_MISMATCH		0xc0000004
//#define STATUS_NO_MEMORY				0xc0000017
#define STATUS_OBJECT_PATH_INVALID		0xc0000039
#define ACCESS_GRANTED_FLAG 0x0012019f
#define SIZE_DEFAULT 0x1000

#define SystemHandleInformation 16
#define ObjectBasicInformation 0
#define ObjectNameInformation 1
#define ObjectTypeInformation 2


// a3 types
#include "a3types_integer.h"


//-----------------------------------------------------------------------------
// windowing utilities

// release the PDB in a chaotic fashion (see sources at top of file)
a3ret a3windowInternalReleasePDB(const a3byte *sdkDirStr, const a3byte *cfgDirStr, const a3byte *projName)
{
	typedef NTSTATUS(NTAPI *_NtQuerySystemInformation)(
		ULONG SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
	);

	typedef NTSTATUS(NTAPI *_NtDuplicateObject)(
		HANDLE SourceProcessHandle,
		HANDLE SourceHandle,
		HANDLE TargetProcessHandle,
		PHANDLE TargetHandle,
		ACCESS_MASK DesiredAccess,
		ULONG Attributes,
		ULONG Options
	);

	typedef NTSTATUS(NTAPI *_NtQueryObject)(
		HANDLE ObjectHandle,
		ULONG ObjectInformationClass,
		PVOID ObjectInformation,
		ULONG ObjectInformationLength,
		PULONG ReturnLength
	);

	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;

	typedef struct _SYSTEM_HANDLE {
		ULONG ProcessId;
		BYTE ObjectTypeNumber;
		BYTE Flags;
		USHORT Handle;
		PVOID Object;
		ACCESS_MASK GrantedAccess;
	} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

	typedef struct _SYSTEM_HANDLE_INFORMATION {
		ULONG HandleCount;
		SYSTEM_HANDLE Handles[1];
	} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

	typedef enum _POOL_TYPE {
		NonPagedPool,
		PagedPool,
		NonPagedPoolMustSucceed,
		DontUseThisType,
		NonPagedPoolCacheAligned,
		PagedPoolCacheAligned,
		NonPagedPoolCacheAlignedMustS
	} POOL_TYPE, *PPOOL_TYPE;

	typedef struct _OBJECT_TYPE_INFORMATION {
		UNICODE_STRING Name;
		ULONG TotalNumberOfObjects;
		ULONG TotalNumberOfHandles;
		ULONG TotalPagedPoolUsage;
		ULONG TotalNonPagedPoolUsage;
		ULONG TotalNamePoolUsage;
		ULONG TotalHandleTableUsage;
		ULONG HighWaterNumberOfObjects;
		ULONG HighWaterNumberOfHandles;
		ULONG HighWaterPagedPoolUsage;
		ULONG HighWaterNonPagedPoolUsage;
		ULONG HighWaterNamePoolUsage;
		ULONG HighWaterHandleTableUsage;
		ULONG InvalidAttributes;
		GENERIC_MAPPING GenericMapping;
		ULONG ValidAccess;
		BOOLEAN SecurityRequired;
		BOOLEAN MaintainHandleCount;
		USHORT MaintainTypeList;
		POOL_TYPE PoolType;
		ULONG PagedPoolUsage;
		ULONG NonPagedPoolUsage;
	} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;


	// data
	a3ret result = 0;
	a3byte pdbName[512] = ".pdb";
	a3i16 pdbNamew[512] = L".pdb";
	a3byte pdb[1024] = ".pdb";
	a3i16 pdbw[1024] = L".pdb", *pdbwp = pdbw;

	WCHAR szSessionKey[CCH_RM_SESSION_KEY + 1] = { 0 };
	DWORD dwSession, dwError;

	// start restart manager session
	dwError = RmStartSession(&dwSession, 0, szSessionKey);
	if (dwError == ERROR_SUCCESS)
	{
		sprintf(pdbName, "%s.pdb", projName);
		sprintf(pdb, "%s\\lib%s\\%s", sdkDirStr, cfgDirStr, pdbName);
		mbstowcs(pdbNamew, pdbName, sizeof(pdbName));
		mbstowcs(pdbw, pdb, sizeof(pdb));

		dwError = RmRegisterResources(dwSession, 1, &pdbwp, 0, 0, 0, 0);
		if (dwError == ERROR_SUCCESS)
		{
			DWORD dwReason;
			UINT nProcInfoNeeded, nProcInfo = SystemHandleInformation;
			RM_PROCESS_INFO procInfo[SystemHandleInformation] = { 0 };

			// get processes holding requested file
			dwError = RmGetList(dwSession, &nProcInfoNeeded, &nProcInfo, procInfo, &dwReason);
			if ((dwError == ERROR_SUCCESS) && nProcInfo)
			{
				// get NT lib handle and functions
				HMODULE ntdll = GetModuleHandleA("ntdll.dll");
				_NtQuerySystemInformation NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
				_NtDuplicateObject NtDuplicateObject = (_NtDuplicateObject)GetProcAddress(ntdll, "NtDuplicateObject");
				_NtQueryObject NtQueryObject = (_NtQueryObject)GetProcAddress(ntdll, "NtQueryObject");

				UINT i, j;
				DWORD pid;
				HANDLE procHandle;
				NTSTATUS status;
				PSYSTEM_HANDLE_INFORMATION handleInfo, handleInfo_tmp;

				SYSTEM_HANDLE handle;
				HANDLE dupHandle = 0;
				POBJECT_TYPE_INFORMATION objectTypeInfo, objectTypeInfo_tmp;
				PUNICODE_STRING objectNameInfo, objectNameInfo_tmp;
				ULONG returnLen;

				for (i = 0; i < nProcInfo; ++i)
				{
					// examine each process
					pid = procInfo[i].Process.dwProcessId;
					procHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid);
					if (procHandle)
					{
						// get a list of all things held by this handle
						handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(SIZE_DEFAULT);
						if (handleInfo)
						{
							status = NtQuerySystemInformation(SystemHandleInformation, handleInfo, SIZE_DEFAULT, &returnLen);
							if (!NT_SUCCESS(status) || status == STATUS_INFO_LENGTH_MISMATCH)
							{
								// retry using proper size
								handleInfo_tmp = handleInfo;
								handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo_tmp, returnLen);
								status = NtQuerySystemInformation(SystemHandleInformation, handleInfo, returnLen, &returnLen);
							}

							if (NT_SUCCESS(status) && handleInfo)
							{
								for (j = 0; j < handleInfo->HandleCount; ++j)
								{
									handle = handleInfo->Handles[j];
									if (handle.ProcessId == pid)
									{
										// found this process, duplicate it to query info
										status = DuplicateHandle(procHandle, (HANDLE)handle.Handle, GetCurrentProcess(), &dupHandle, PROCESS_QUERY_INFORMATION, 0, 0);
										if (dupHandle && NT_SUCCESS(status))
										{
											objectTypeInfo = (POBJECT_TYPE_INFORMATION)malloc(SIZE_DEFAULT);
											status = NtQueryObject(dupHandle, ObjectTypeInformation, objectTypeInfo, SIZE_DEFAULT, &returnLen);
											if (!NT_SUCCESS(status) || status == STATUS_OBJECT_PATH_INVALID)
											{
												objectTypeInfo_tmp = objectTypeInfo;
												objectTypeInfo = (POBJECT_TYPE_INFORMATION)realloc(objectTypeInfo_tmp, returnLen);
												status = NtQueryObject(dupHandle, ObjectTypeInformation, objectTypeInfo, returnLen, &returnLen);
											}

											// looking for object of 'File' type
											if (NT_SUCCESS(status) && objectTypeInfo && wcsstr(objectTypeInfo->Name.Buffer, L"File") != 0)
											{
												// query object can hang if this flag is set
												if (handle.GrantedAccess != ACCESS_GRANTED_FLAG && GetFileType(dupHandle) == FILE_TYPE_DISK)
												{
													objectNameInfo = (PUNICODE_STRING)malloc(SIZE_DEFAULT);
													status = NtQueryObject(dupHandle, ObjectNameInformation, objectNameInfo, SIZE_DEFAULT, &returnLen);
													if (!NT_SUCCESS(status) || status == STATUS_OBJECT_PATH_INVALID)
													{
														objectNameInfo_tmp = objectNameInfo;
														objectNameInfo = (PUNICODE_STRING)realloc(objectNameInfo_tmp, returnLen);
														status = NtQueryObject(dupHandle, ObjectNameInformation, objectNameInfo, returnLen, &returnLen);
													}
													CloseHandle(dupHandle);

													// looking for pdb
													if (NT_SUCCESS(status) && objectNameInfo && wcsstr(objectNameInfo->Buffer, pdbNamew) != 0)
													{
														// the final step: duplicate and close handle
														DuplicateHandle(procHandle, (PVOID)handle.Handle, GetCurrentProcess(), &dupHandle, 0, 0, DUPLICATE_CLOSE_SOURCE);
														CloseHandle(dupHandle);

														// create and immediately close pdb file
														dupHandle = CreateFileA(pdb, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, 0);
														if (dupHandle)
															result = CloseHandle(dupHandle);

														// done
														j = handleInfo->HandleCount;
													}

													// done with object name
													free(objectNameInfo);
												}
												else
													CloseHandle(dupHandle);
											}
											else
												CloseHandle(dupHandle);

											// done with object type
											free(objectTypeInfo);
										}
									}
								}
							}

							// done
							free(handleInfo);
							CloseHandle(procHandle);
						}
					}
				}
			}
			// if no processes are holding the file, we must be done... right?
			else if (nProcInfo == 0)
				result = 1;
		}

		// end session
		RmEndSession(dwSession);
	}

	// done, return final status
	return result;
}


//-----------------------------------------------------------------------------


#endif	// (defined _WINDOWS || defined _WIN32)