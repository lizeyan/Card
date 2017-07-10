import re
import threading
import sys
import logging


class CardCommunicator(threading.Thread):
    """
    input protocol: "(command name in upper case)\s+(data)?\n"
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
            if command in self.handler_dict:
                self.handler_dict[command](match.group("data"))

    def send(self, msg):
        pass


