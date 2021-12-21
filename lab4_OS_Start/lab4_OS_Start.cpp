#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>

HANDLE CreateNewProcess(const std::string& exePath, const std::string& logName) {

	SECURITY_ATTRIBUTES securityAttributes = { sizeof(securityAttributes), nullptr, true };

	HANDLE logFileHandle = CreateFile(logName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, &securityAttributes,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = logFileHandle;
	startupInfo.hStdError = NULL;
	startupInfo.hStdInput = NULL;

	ZeroMemory(&processInformation, sizeof(processInformation));

	if(CreateProcess(exePath.c_str(), NULL, NULL, NULL, true, NULL, NULL, NULL, &startupInfo, &processInformation))
		return processInformation.hProcess;

	return nullptr;
}

int main()
{
	std::cout << "ITS STARTING" << std::endl;

	const int pageSize = 4096;
	const int pageCount = 12;
	const int processCount = 14;
	const int hProcessCount = processCount / 2;

	HANDLE writeSemaphores[pageCount];
	HANDLE readSemaphores[pageCount];
	HANDLE IOMutex = CreateMutex(NULL, false, "IOMutex");
	HANDLE fileHandle = CreateFile("D:\\lab4\\text.txt", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE mapFile = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, pageSize * pageCount, "MAPPING");
	HANDLE processHandles[processCount];

	for (int i = 0; i < pageCount; i++)
	{
		std::string semaphoreName = "writeSemaphore_" + std::to_string(i);
		writeSemaphores[i] = CreateSemaphore(NULL, 1, 1, semaphoreName.c_str());
		semaphoreName = "readSemaphore_" + std::to_string(i);
		readSemaphores[i] = CreateSemaphore(NULL, 0, 1, semaphoreName.c_str());
	}

	for (int i = 0; i < hProcessCount; i++)
	{
		std::string logName = "D:\\lab4\\readLogs\\readLog_" + std::to_string(i) + ".txt";
		processHandles[i] = CreateNewProcess("C:\\Users\\ZuniXX\\source\\repos\\lab4_OS_W\\Debug\\lab4_OS_W.exe",
			logName);
	}

	for (int i = 0; i < hProcessCount; i++)
	{
		std::string logName = "D:\\lab4\\writeLogs\\writeLog_" + std::to_string(i) + ".txt";
		processHandles[i + hProcessCount] = CreateNewProcess("C:\\Users\\ZuniXX\\source\\repos\\lab4_OS_R\\Debug\\lab4_OS_R.exe",
			logName);
	}

	std::cout << "WAIT!" << std::endl;

	WaitForMultipleObjects(processCount, processHandles, true, INFINITE);
	std::cout << "ITS OVER" << std::endl;

	CloseHandle(IOMutex);
	CloseHandle(mapFile);
	CloseHandle(fileHandle);
	for (int i = 0; i < pageCount; i++)
	{
		CloseHandle(writeSemaphores[i]);
		CloseHandle(readSemaphores[i]);
	}
	return 0;
}