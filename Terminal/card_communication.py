import re
import threading
import sys
import logging


class CardCommunicator(threading.Thread):
    """
    communication protocol: "(command name in upper case)\s+(data)?\n"
    known instruction:
        ARRIVAL {uid} # new card selected
        LEAVE  # card missing
        APPENDLOG {timestamp: uint32} {+/-: bit} {amount: uint32(real amount * 100)} {location: 32bytes}  # write a log
        LOG {timestamp: uint32} {+/-: bit(0/1)} {amount: uint32(real amount * 100)} {location: 32bytes string}  # send a log to terminal
        CLEAR # clean all log

    """
    def __init__(self):
        self.handler_dict = {}
        self.input_line_parser = re.compile(r"(?P<command>[A-Z]+)\s+(?P<data>.*)\n?")
        threading.Thread.__init__(self)

    def register(self, command: str, handler):
        self.handler_dict[command.upper()] = handler
        logging.debug("register {func} to {command}".format(func=handler.__name__, command=command))

    def run(self):
        for line in sys.stdin:
            match = self.input_line_parser.match(line)
            if not match:
                continue
            command = match.group("command")
            data = match.group("data")
            if command in self.handler_dict:
                if data != "":
                    self.handler_dict[command](data)
                else:
                    self.handler_dict[command]()

    def send(self, msg):
        pass


