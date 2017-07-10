import logging
FORMAT = '%(asctime)-15s %(message)s'
logging.basicConfig(handlers=[logging.FileHandler("terminal.log", encoding="utf-8")], level=logging.DEBUG, format=FORMAT)
HOST = "http://123.207.163.49:8000/"
