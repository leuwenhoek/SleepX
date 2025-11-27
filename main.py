import multiprocessing
import os

def run_sleep_detector():
    # Execute the file as a separate Python interpreter process
    os.system("python sleep_detector.py") 

def run_app():
    os.system("python .\web\\app.py")

# def run_IOT(cmd):
#     # Remember to include your port argument!
#     os.system(cmd)


if __name__ == '__main__':
    print("Starting all components in the same terminal...")
    
    # 1. Start the processes
    p1 = multiprocessing.Process(target=run_sleep_detector)
    p2 = multiprocessing.Process(target=run_app)
    # IOT_process = multiprocessing.Process(target=run_IOT("python .\IoT\\display.py"))

    p1.start()
    p2.start()
    # IoT.start()

    # 2. Wait for all processes to finish (optional)
    p1.join()
    p2.join()
    # IoT_process.join()

    print("All components shut down.")
