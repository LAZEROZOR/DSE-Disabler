#include <ntifs.h>

ULONG EnableDSECode = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
ULONG DisableDSECode = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);

struct Request
{
    PVOID address;
};
typedef struct Request Request;


VOID DisableDSE(PVOID addr)
{

    if (!MmIsAddressValid(addr)) 
    {
        return;
    }

    if (((ULONG_PTR)addr & (sizeof(LONG) - 1)) != 0)
    {
        return;
    }


    InterlockedExchange((volatile LONG*)addr, 0);
}

VOID EnableDSE(PVOID addr)
{

    if (!MmIsAddressValid(addr)) 
    {
        return;
    }

    if (((ULONG_PTR)addr & (sizeof(LONG) - 1)) != 0)
    {
        return;
    }


    InterlockedExchange((volatile LONG*)addr, 6);
}

NTSTATUS createCloseCom(PDEVICE_OBJECT device_object, PIRP irp)
{
    UNREFERENCED_PARAMETER(device_object);
    irp->IoStatus.Status = STATUS_SUCCESS;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return irp->IoStatus.Status;
}

NTSTATUS device_control(PDEVICE_OBJECT device_object, PIRP irp) 
{
	UNREFERENCED_PARAMETER(device_object);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PIO_STACK_LOCATION stack_irp = IoGetCurrentIrpStackLocation(irp);

    if (!stack_irp)
    { 
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(Request);

        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }

    if (!irp->AssociatedIrp.SystemBuffer)
    {
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(Request);

        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }

    Request* request = (Request*)(irp->AssociatedIrp.SystemBuffer);

    if (stack_irp->Parameters.DeviceIoControl.InputBufferLength <= 0) 
    {
        status = STATUS_BUFFER_TOO_SMALL;
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(Request);

        IoCompleteRequest(irp, IO_NO_INCREMENT);
		return status;
    }

	const ULONG control_code = stack_irp->Parameters.DeviceIoControl.IoControlCode;

    if (control_code == EnableDSECode)
    {
		EnableDSE(request->address);
        status = STATUS_SUCCESS;
    }

    if (control_code == DisableDSECode)
    {
		DisableDSE(request->address);
		status = STATUS_SUCCESS;
    }

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = sizeof(Request);

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS DriverMain(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    UNICODE_STRING device_name;
    RtlInitUnicodeString(&device_name, L"\\Device\\DSEdriver");
    PDEVICE_OBJECT device_object;
    IoCreateDevice(DriverObject, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
    UNICODE_STRING symbolic_link;
    RtlInitUnicodeString(&symbolic_link, L"\\DosDevices\\DSEdriver");
    IoCreateSymbolicLink(&symbolic_link, &device_name);

    SetFlag(device_object->Flags, DO_BUFFERED_IO);
    DriverObject->MajorFunction[IRP_MJ_CREATE] = createCloseCom;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = createCloseCom;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = device_control;

    ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry()
{
    UNICODE_STRING driver_name;
    RtlInitUnicodeString(&driver_name, L"\\Driver\\DSEdriver");


    return IoCreateDriver(&driver_name, &DriverMain);
}
