import asyncio  # Run asynchronous BLE tasks.
import struct  # Decode binary bytes into Python numbers.

from bleak import BleakClient, BleakScanner  # Import BLE connection and discovery tools.

TEMPERATURE_UUID = "7e400002-b5a3-f393-e0a9-e50e24dcca9e"  # Match the temperature characteristic UUID in the Nano sketch.


def on_temperature(characteristic, data):  # Called for each notification; data contains the received bytes.
    if len(data) != 4:  # A 32-bit float must contain exactly four bytes.
        print(f"Unexpected payload: {data.hex()}")  # Show malformed bytes in hexadecimal.
        return  # Leave this function early.

    temperature_c = struct.unpack("<f", data)[0]  # Decode one little-endian float; take it from the returned tuple.
    print(f"Temperature: {temperature_c:.1f} C")  # Display Celsius with one decimal place.


async def main():  # Define the asynchronous receiver task.
    print("Scanning for EverCold...")  # Report that discovery is starting.
    device = await BleakScanner.find_device_by_name(  # Wait for discovery without blocking other async work.
        "EverCold", timeout=20.0  # Search by advertised name for up to 20 seconds.
    )

    if device is None:  # Handle a scan that found no matching device.
        print("EverCold not found. Check power and Bluetooth.")  # Explain why the receiver is stopping.
        return  # Leave this function early.

    disconnected = asyncio.Event()  # Create a signal that can wake the task on disconnect.

    async with BleakClient(  # Connect on entry; disconnect automatically when leaving this block.
        device,  # Use the device discovered above.
        disconnected_callback=lambda client: disconnected.set(),  # Signal the event if the BLE connection closes.
    ) as client:
        print(f"Connected to {device.address}")  # Display the connected device address.
        await client.start_notify(TEMPERATURE_UUID, on_temperature)  # Subscribe and route updates to our callback.
        print("Receiving temperatures. Press Ctrl+C to stop.")  # Report readiness and how to exit.

        await disconnected.wait()  # Keep receiving until disconnected; async callbacks can still run.
        print("BLE disconnected.")  # Report that the connection ended.


if __name__ == "__main__":  # Start only when run directly, rather than imported.
    try:  # Allow a clean exit when Ctrl+C is pressed.
        asyncio.run(main())  # Run the event loop for our asynchronous task.
    except KeyboardInterrupt:  # Catch the Ctrl+C interruption.
        print("Receiver stopped.")  # Report a clean user-requested stop.
