#include <wdm.h>

/// <summary>
/// Driver unload function 
/// </summary>
void DriverUnload(_In_ PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	KdPrint(("DriverUnload is executing...\n"));

	/*
	* TODO: Free any resources allocated by the driver before the driver
	* is unloaded.
	* 
	* A driver MUST free its resources, as the kernel will not do this for us.
	* A driver resource leak is only resolved by rebooting the system
	*/
}

/// <summary>
/// Entry-point for the driver
/// </summary>
extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject,
	_In_ PUNICODE_STRING pucsRegistryPath)
{
	/*
	* At this point, we are at IRQL_PASSIVE_LEVEL
	* 
	* pDriverObject points to a driver object allocated by the I/O manager and
	* pucsRegistryPath is populated with the registry key path for this driver
	* under CurrentControlSet\Services
	* 
	* If pucsRegistryPath is saved, do so via copy. The pointer here becomes
	* invalid after DriverEntry returns
	*/
	UNREFERENCED_PARAMETER(pucsRegistryPath);

	/*
	* Unlikely, but good to validate
	*/
	if (nullptr == pDriverObject)
	{
		KdPrint(("DriverEntry received NULL driver object\n"));
		return STATUS_INVALID_PARAMETER_1;
	}

	/*
	* Driver object is then populated with per-driver standard routines. The system
	* provides a default callback for each unless overridden here.
	* 
	* In our case, we specify a DriverUnload callback to be executed before
	* the driver is unloaded
	*/
	pDriverObject->DriverUnload = DriverUnload;

	/*
	* Print to debug output, visible to a kernel debugger. Note that the double
	* parentheses are necessary
	*/
	KdPrint(("DriverEntry is executing...\n"));

	/*
	* Now attempt to retrieve and print the Windows version as an example
	*/

	OSVERSIONINFOW osVersionInfo;
	// Zero osVersionInfo, uninitialized variables are dangerous in kernel mode
	RtlZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));

	// Structure size field determines return value from RtlGetVersion
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	NTSTATUS ntStatus{ RtlGetVersion(&osVersionInfo) };

	/*
	* Always check the return value, no matter how simple a call may be. Ignoring
	* errors in kernel space has a higher risk and consequence
	*/
	if (false == NT_SUCCESS(ntStatus))
	{
		KdPrint(("DriverEntry failed RtlGetVersion (%ld)\n", ntStatus));
		return ntStatus;
	}

	// Print out the Windows build we are running
	KdPrint(("DriverEntry detected Windows %lu.%lu Build %lu\n",
		osVersionInfo.dwMajorVersion, osVersionInfo.dwMinorVersion,
		osVersionInfo.dwBuildNumber));

	// Return STATUS_SUCCESS to indicate that our driver has loaded successfully
	return STATUS_SUCCESS;
}
