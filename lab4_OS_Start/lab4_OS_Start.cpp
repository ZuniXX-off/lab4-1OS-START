#include <iostream>
#include <ctime>
#include <Windows.h>
#include <string>

HANDLE CreateProcessNew(std::wstring exePath, std::wstring logName);

int main() {

	const int pageCount = 12;
	const int processCount = 14;
	const int writerCount = processCount / 2;
	const int readerCount = processCount / 2;
	const int pageSize = 4096;

	HANDLE hFile = CreateFile(L"D:\\lab4\\text.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {

		HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, NULL, pageCount * pageSize, L"mappedFile");

		if (hFileMapping != INVALID_HANDLE_VALUE) {
			HANDLE IOMutex = CreateMutex(NULL, false, L"IOMutex");
			HANDLE readSemaphore[pageCount], writeSemaphore[pageCount];
			HANDLE processHandles[processCount];
			
			for (int i = 0; i < pageCount; i++) {
				std::wstring semaphoreName = L"readSemaphore_" + std::to_wstring(i);
				readSemaphore[i] = CreateSemaphore(NULL, 0, 1, semaphoreName.data());
				semaphoreName = L"writeSemaphore_" + std::to_wstring(i);
				writeSemaphore[i] = CreateSemaphore(NULL, 1, 1, semaphoreName.data());
				WaitForSingleObject(readSemaphore[i], INFINITE);
			}

			for (int i = 0; i < writerCount; i++) {
				std::wstring logName = L"D:\\lab4\\writeLogs\\writeLog_" + std::to_wstring(i) + L".txt";

				HANDLE handleProcess = CreateProcessNew(L"C:\\Users\\ZuniXX\\source\\repos\\lab4_OS_W\\Debug\\lab4_OS_W.exe",
					logName);

				processHandles[i] = handleProcess;
			}

			for (int i = 0; i < readerCount; i++) {
				std::wstring logName = L"D:\\lab4\\readLogs\\readLog_" + std::to_wstring(i) + L".txt";

				HANDLE handleProcess = CreateProcessNew(L"C:\\Users\\ZuniXX\\source\\repos\\lab4_OS_R\\Debug\\lab4_OS_R.exe",
					logName);

				processHandles[readerCount + i] = handleProcess;
			}

			WaitForMultipleObjects(processCount, processHandles, true, INFINITE);
			std::cout << "Работа завершена" << std::endl;

			for (int i = 0; i < pageCount; i++) {
				CloseHandle(readSemaphore[i]);
				CloseHandle(writeSemaphore[i]);
			}

			CloseHandle(IOMutex);
			CloseHandle(hFileMapping);

		}
		else {
			std::cout << "Ошибка проецирования файла, код: " << GetLastError() << std::endl;
		}

		CloseHandle(hFile);

	}
	else {
		std::cout << "Ошибка создания файла, код: " << GetLastError() << std::endl;
	}

	return 0;
}

HANDLE CreateProcessNew(std::wstring exePath, std::wstring logName) {
	HANDLE handleLogFile = CreateFile(logName.data(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	STARTUPINFO startupInformation;
	ZeroMemory(&startupInformation, sizeof(STARTUPINFO));
	startupInformation.cb = sizeof(STARTUPINFO);
	startupInformation.hStdOutput = handleLogFile;
	startupInformation.hStdError = NULL;
	startupInformation.hStdInput = NULL;
	startupInformation.dwFlags = STARTF_USESTDHANDLES;

	PROCESS_INFORMATION processInformation;
	ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));

	if (!CreateProcess(exePath.data(), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &startupInformation, &processInformation)) {
		return nullptr;
	}
	return processInformation.hProcess;
}
