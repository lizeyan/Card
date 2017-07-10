from datetime import datetime
from tkinter import *
import logging
import settings
import requests
from card_communication import CardCommunicator
from data_session import *


class Terminal(object):
    def __init__(self):
        self.root = Tk()
        self.frame = Frame(self.root)
        self.frame.grid()
        self.data_session = DataSession()
        self.card_reader = CardCommunicator()
        self.card_reader.setDaemon(True)
        self.card_reader.start()

        # setup login frame
        self.login_frame = Frame(self.frame)
        Label(self.login_frame, text="User Name").grid(row=0, column=0)
        self.terminal_name = StringVar()
        self.terminal_name.set("root")
        self.terminal_name_edit = Entry(self.login_frame, text=self.terminal_name)
        self.terminal_name_edit.grid(row=0, column=1)
        Label(self.login_frame, text="password").grid(row=1, column=0)
        self.terminal_password = StringVar()
        self.terminal_password.set("123xuyimiao")
        self.terminal_password_edit = Entry(self.login_frame, show="*", text=self.terminal_password)
        self.terminal_password_edit.grid(row=1, column=1)
        self.terminal_login_btn = Button(self.login_frame, text="Login", command=self.try_to_login)
        self.terminal_login_btn.grid(row=2, column=0, columnspan=2)
        self.login_frame.grid(row=0, column=0)

        # setup main frame
        self.main_frame = Frame(self.frame)
        self.create_btn = Button(self.main_frame, text="Create")
        self.update_btn = Button(self.main_frame, text="Update")
        self.delete_btn = Button(self.main_frame, text="Delete")
        self.consume_btn = Button(self.main_frame, text="Consume")
        self.recharge_btn = Button(self.main_frame, text="Recharge")
        self.delta_amount = DoubleVar()
        self.amount_edit = Entry(self.main_frame, text=self.delta_amount)
        self.student_name = StringVar()
        self.student_name.set(u"李则言")
        self.student_id = StringVar()
        self.student_id.set("2014011292")
        self.valid_duration_start = StringVar()
        self.valid_duration_start.set(str(datetime.now()))
        self.valid_duration_end = StringVar()
        self.valid_duration_end.set(str(datetime.fromtimestamp(datetime.now().timestamp() + 100000)))
        self.balance = DoubleVar()
        self.balance.set(0)
        self.card_read_status = StringVar()
        self.card_read_status.set("No Card")

        self.create_btn.grid(row=0, column=0)
        self.delete_btn.grid(row=1, column=0)
        self.update_btn.grid(row=2, column=0)
        self.amount_edit.grid(row=3, column=0)
        self.consume_btn.grid(row=4, column=0)
        self.recharge_btn.grid(row=5, column=0)
        Label(self.main_frame, textvariable=self.student_name).grid(row=0, column=1)
        Label(self.main_frame, textvariable=self.student_id).grid(row=1, column=1)
        Label(self.main_frame, textvariable=self.valid_duration_start).grid(row=2, column=1)
        Label(self.main_frame, textvariable=self.valid_duration_end).grid(row=3, column=1)
        Label(self.main_frame, textvariable=self.balance).grid(row=4, column=1)

        # card reader setting
        self.card_reader.register("ARRIVAL", self.new_card_arrival_handler)

        self.frame.mainloop()

    def try_to_login(self):
        self.data_session.authentication(self.terminal_name.get(), self.terminal_password.get())
        if self.data_session.token != "":
            self.show_main_frame()

    def show_login_frame(self):
        self.main_frame.grid_forget()
        self.login_frame.grid()

    def show_main_frame(self):
        self.login_frame.grid_forget()
        self.main_frame.grid()

    def new_card_arrival_handler(self, uid):
        info = self.data_session.query_card(uid)
        self.student_name.set(info["name"])
        self.student_id.set(info["student_id"])
        self.valid_duration_start.set(info["begin_time"])
        self.valid_duration_end.set(info["end_time"])
        self.balance.set(info["card_money"])

if __name__ == '__main__':
    logging.debug("Application start")
    terminal = Terminal()

