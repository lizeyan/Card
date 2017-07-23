import re
import threading
import serial
import serial.tools.list_ports
import sys
import logging


class CardCommunicator(threading.Thread):
    """
    communication protocol: "(command name in upper case)\s+(data)?\n"
    known instruction:
        ARRIVAL {uid} # new card selected
        LEAVE  # card missing
        SMALLANSWER {answer: uint32} {answer: uint32} {answer: uint32} {answer: uint32}  # return now money in small card
        ASKFORLOG  # when this message is received, the card reader should read and send the logs
        APPENDLOG {timestamp: uint32} {+/-: bit} {amount: uint32(real amount * 100)} {location: 32bytes}  # write a log
        LOG {timestamp: uint32} {+/-: bit(0/1)} {amount: uint32(real amount * 100)} {location: 32bytes string}  # send a log to terminal
        SMALLMONEY {answer: uint32} {answer: uint32} {answer: uint32} {answer: uint32}
        SMALLQUERY # get now money in small wallet
        CLEAR # clean all log
        ACCESSACCEPTED
        ACCESSDENIED

    """
    def __init__(self):
        self.handler_dict = {}
        self.input_line_parser = re.compile(r"(?P<command>[A-Z]+)\s*(?P<data>[^\n\r]*)\s*?")
        try:
            serial_comports = serial.tools.list_ports.comports()
            for comport in serial_comports:
                print(comport.description)
                if re.match("USB*", comport.description):
                    self.serial = serial.Serial(comport.device)
                    break
        except IndexError or FileNotFoundError:
            raise RuntimeError("Unable to open the first Serial Port")
        logging.debug("Open Serial {name}".format(name=self.serial.port))
        threading.Thread.__init__(self)

    def register(self, command: str, handler):
        self.handler_dict[command.upper()] = handler
        logging.debug("register {func} to {command}".format(func=handler.__name__, command=command))

    def remove(self, command: str):
        if command in self.handler_dict:
            del self.handler_dict[command.upper()]
        logging.debug("remove {command}".format(command=command))

    def run(self):
        while True:
            line = self.serial.readline()
            line = line.decode("utf-8").rstrip("\r\n")
            logging.info("received:" + line)
            match = self.input_line_parser.match(line)
            if not match:
                continue
            command = match.group("command")
            data = match.group("data")
            logging.info("command:" + command)
            logging.info("data:" + data)
            if command in self.handler_dict:
                if data != "":
                    self.handler_dict[command](data)
                else:
                    self.handler_dict[command]()

    def send(self, msg):
        logging.info("send:" + msg)
        self.serial.write((msg + "\n").encode("utf-8"))

