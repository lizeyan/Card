from datetime import datetime, timedelta
import time
from tkinter import Tk, StringVar, DoubleVar, messagebox, font, Button
from tkinter.ttk import Label, Entry, Frame, Style
import logging

import settings
import requests
from card_communication import CardCommunicator
from data_session import *


class Terminal(object):
    def __init__(self):
        self.root = Tk()
        self.root.title("Terminal")
        self.root.style = Style()
        self.root.style.theme_use("classic")
        self.custom_font = font.Font(family="Microsoft Yahei", size=16)
        self.frame = Frame(self.root)
        self.frame.grid()
        self.data_session = DataSession()
        self.card_communicator = CardCommunicator()
        self.card_communicator.setDaemon(True)

        # setup login frame
        self.login_frame = Frame(self.frame)
        Label(self.login_frame, text="User Name", font=self.custom_font).grid(row=0, column=0)
        self.terminal_name = StringVar()
        self.terminal_name.set("root")
        self.terminal_name_edit = Entry(self.login_frame, text=self.terminal_name, font=self.custom_font)
        self.terminal_name_edit.grid(row=0, column=1)
        Label(self.login_frame, text="password", font=self.custom_font).grid(row=1, column=0)
        self.terminal_password = StringVar()
        self.terminal_password.set("123xuyimiao")
        self.terminal_password_edit = Entry(self.login_frame, show="*", text=self.terminal_password, font=self.custom_font)
        self.terminal_password_edit.grid(row=1, column=1)
        self.terminal_login_btn = Button(self.login_frame, text="Login", command=self.try_to_login, font=self.custom_font)
        self.terminal_login_btn.grid(row=2, column=0, columnspan=2)
        self.login_frame.grid(row=0, column=0)

        # setup main frame
        self.main_frame = Frame(self.frame)
        self.create_btn = Button(self.main_frame, text="Create", command=self.card_create, font=self.custom_font, width=15)
        self.update_btn = Button(self.main_frame, text="Update", command=self.update_card, font=self.custom_font, width=15)
        self.delete_btn = Button(self.main_frame, text="Delete", command=self.card_delete, font=self.custom_font, width=15)
        self.consume_btn = Button(self.main_frame, text="Consume", command=self.card_consume, font=self.custom_font, width=15)
        self.recharge_btn = Button(self.main_frame, text="Recharge", command=self.card_recharge, font=self.custom_font, width=15)
        self.view_log_btn = Button(self.main_frame, text="View Log", command=self.card_log_show, font=self.custom_font, width=15)
        self.delta_amount = DoubleVar()
        self.delta_amount_edit = Entry(self.main_frame, text=self.delta_amount, font=self.custom_font, width=15)
        self.student_name = StringVar()
        self.student_id = StringVar()
        self.valid_duration_start = StringVar()
        self.valid_duration_end = StringVar()
        self.status_string = StringVar()
        self.balance = DoubleVar()

        self.create_btn.grid(row=0, column=0)
        self.delete_btn.grid(row=1, column=0)
        self.update_btn.grid(row=2, column=0)
        self.delta_amount_edit.grid(row=3, column=0)
        self.consume_btn.grid(row=4, column=0)
        self.recharge_btn.grid(row=5, column=0)
        self.view_log_btn.grid(row=6, column=0)
        Label(self.main_frame, textvariable=self.student_name, font=self.custom_font, width=20).grid(row=0, column=1)
        Entry(self.main_frame, textvariable=self.student_id, font=self.custom_font, width=20).grid(row=1, column=1)
        Label(self.main_frame, textvariable=self.valid_duration_start, font=self.custom_font, width=20).grid(row=2, column=1)
        Label(self.main_frame, textvariable=self.valid_duration_end, font=self.custom_font, width=20).grid(row=3, column=1)
        Label(self.main_frame, textvariable=self.balance, font=self.custom_font, width=20).grid(row=4, column=1)

        Label(self.main_frame, textvariable=self.status_string, font=self.custom_font, width=20 + 15).grid(row=7, column=0, columnspan=2)

        # card reader setting
        self.uid = ""
        self.student_info = {}
        self.card_communicator.register("ARRIVAL", self.card_arrival_handler)
        self.card_communicator.register("LEAVE", self.card_leave_handler)
        self.card_communicator.register("SMALLANSWER", self.card_small_wallet_handler)

        self.card_leave_handler()
        self.status_string.set("Application start")
        self.frame.mainloop()

    def try_to_login(self):
        self.data_session.authentication(self.terminal_name.get(), self.terminal_password.get())
        if self.data_session.token != "":
            self.show_main_frame()
            self.card_communicator.start()

    def show_login_frame(self):
        self.main_frame.grid_forget()
        self.login_frame.grid()

    def show_main_frame(self):
        self.login_frame.grid_forget()
        self.main_frame.grid()

    def card_arrival_handler(self, uid):
        logging.debug("Card {uid} arrived.".format(uid=uid))
        self.student_info = self.data_session.query_card_by_uid(uid)
        self.uid = uid
        self.query_access()
        if self.student_info != {}:
            self.student_name.set(self.student_info["name"])
            self.student_id.set(self.student_info["student_id"])
            self.valid_duration_start.set(self.student_info["begin_time"])
            self.valid_duration_end.set(self.student_info["end_time"])
            self.balance.set(self.student_info["card_money"])
        self.status_string.set("Card {uid} arrived.".format(uid=uid))

    def query_access(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        if self.data_session.access(self.uid):
            self.card_communicator.send("ACCESSACCEPTED")
        else:
            self.card_communicator.send("ACCESSDENIED")

    def card_leave_handler(self):
        logging.debug("card {uid} left.".format(uid=self.uid))
        self.status_string.set("Card {uid} left.".format(uid=self.uid))
        self.uid = ""
        self.student_name.set("Name")
        self.student_id.set("Id")
        self.valid_duration_start.set("Valid Duration")
        self.valid_duration_end.set("Valid Duration")
        self.balance.set(0.0)

    def update_card(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        time_point = datetime.strptime(self.student_info["end_time"], "%Y-%m-%dT%H:%M:%S")
        time_point += timedelta(weeks=18)
        self.data_session.put_card(self.uid, self.student_info["url"], {"end_time": time_point.strftime("%Y-%m-%dT%H:%M:%S")})
        self.card_arrival_handler(self.uid)
        self.status_string.set("Updated valid duration {begin}, {end}".format(begin=self.student_info["begin_time"], end=self.student_info["end_time"]))

    def card_recharge(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        self.data_session.increase_money(self.uid, self.delta_amount.get())
        self.card_arrival_handler(self.uid)
        self.status_string.set(u"Recharge ￥{amount}".format(amount=self.delta_amount.get()))

    def card_consume(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        self.data_session.decrease_money(self.uid, self.delta_amount.get())
        self.card_arrival_handler(self.uid)
        self.status_string.set(u"Consume ￥{amount}".format(amount=self.delta_amount.get()))

    def card_small_wallet_recharge(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        increase_money = self.delta_amount.get()
        response = self.data_session.decrease_money(self.uid, increase_money)
        if response['status'] == 'success':
            self.card_communicator.send("SMALLMONEY 0 " + str(int(increase_money * 100)))
        else:
            messagebox.showerror("ERROR", "There is not enough money.")

    def card_small_wallet_consume(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        decrease_money = self.delta_amount.get()
        self.card_communicator.send("SMALLMONEY 1 " + str(int(decrease_money * 100)))

    def card_small_wallet_handler(self, data):
        sp = data.strip().split()
        if sp[0] == '0':
            now_money = float(sp[1]) / 100
            messagebox.showinfo(u"info", "Success, now small wallet money ￥{amount}".format(amount=now_money))
        else:
            messagebox.showerror("ERROR", "There is not enough money in small wallet.")

    def card_delete(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        self.data_session.delete_card(self.uid, self.student_info["url"])
        self.status_string.set("Delete Card {uid}".format(uid=self.uid))

    def card_create(self):
        if self.uid == "":
            messagebox.showerror("ERROR", "There is no card.")
            return
        self.data_session.create_card(self.student_id.get(), self.uid)
        self.status_string.set("Create Card {uid} for {student}".format(uid=self.uid, student=self.student_id.get()))
        self.card_arrival_handler(self.uid)

    def card_log_show(self):
        logs = []
        self.card_communicator.register("LOG", lambda log: logs.append(log))
        while len(logs) < 5:
            time.sleep(0.1)
        self.card_communicator.remove("LOG")
        popup = Tk()
        popup.title("Logs")
        for idx, log in enumerate(logs):
            Label(popup, text=log, font=self.custom_font).grid(row=idx, column=0)

if __name__ == '__main__':
    logging.debug("Application start")
    terminal = Terminal()

