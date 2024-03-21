import os
import signal
import threading
import time

def signal_handler(sig, frame):
    print(f"Thread {threading.current_thread().name} migrated to CPU core {os.sched_getcpu()}")

def worker_thread():
    while True:
        # Perform some work
        time.sleep(1)

def main():
    # Register the signal handler for SIGUSR1
    signal.signal(signal.SIGUSR1, signal_handler)

    # Create and start the worker thread
    worker = threading.Thread(target=worker_thread)
    worker.start()

    print(f"Worker thread started with PID: {os.getpid()}")
    print("Use the following command to trigger thread migration:")
    print(f"kill -SIGUSR1 {os.getpid()}")

    # Wait for the worker thread to complete (which is never in this case)
    worker.join()

if __name__ == "__main__":
    main()
