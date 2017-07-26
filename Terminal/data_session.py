import requests
import settings
import logging
import atexit
import os
from datetime import datetime
from encrypt_pickle import *
import json


class DataSession(object):
    RECENT_ACCESS_LOG_CACHE_PATH = "ACCESS_LOG.pkl"
    RECENT_ACCESS_LOG_CACHE_MAX_LENGTH = 1000

    def __init__(self):
        self.token = ""
        self.access_log = load_decrypt(self.RECENT_ACCESS_LOG_CACHE_PATH)
        if self.access_log is None:
            self.access_log = {}  # key: uid, value: access_time(datetime))
        atexit.register(self.exit)

    def exit(self):
        logging.debug("dump to " + self.RECENT_ACCESS_LOG_CACHE_PATH)
        dump_encrypt(self.access_log, self.RECENT_ACCESS_LOG_CACHE_PATH)

    def authentication(self, user_name: str, password: str) -> dict:
        """
        :param user_name:
        :param password:
        :return: token. empty string if authentication failed.
        """
        url = settings.HOST + "api-token-auth/"
        rsp = requests.post(url, data={"username": user_name, "password": password}, verify=False)
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        if rsp.status_code == 200:
            self.token = rsp.json()["token"]
        else:
            self.token = ""
        return rsp.json()

    def access(self, uid: str) -> (bool, str):
        """
        :param uid:
        :return: is allowed or not to access, error message
        """
        url = settings.HOST + "card/access/"
        try:
            rsp = requests.post(url, json={"card_id": uid}, params={"jwt": self.token}, timeout=0.5, verify=False)
            logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
            result = rsp.status_code == 200
        except:
            result = uid in self.access_log
            print(self.access_log)
            logging.debug("local access {result}".format(result="accepted" if result else "denied"))
        if result:
            self.access_log[uid] = datetime.now()
            if len(self.access_log) > self.RECENT_ACCESS_LOG_CACHE_MAX_LENGTH:
                for k, v in self.access_log.items():
                    if (datetime.now() - v).total_seconds() > 86400:
                        del self.access_log[k]
        return result

    def query_card_by_uid(self, uid: str) -> dict:
        """
        :param uid:
        :return: info dict of the student
        """
        url = settings.HOST + "card/"
        rsp = requests.get(url, params={"card_id": uid, "jwt": self.token}, timeout=1, verify=False)
        logging.debug("Response of {method} {url}: {rsp}".format(method="GET", url=url, rsp=rsp.text))
        results = rsp.json()["results"]
        if len(results) > 0:
            return rsp.json()["results"][0]
        else:
            return {}

    def query_card_by_student_id(self, student_id: str) -> dict:
        """
        :param student_id:
        :return: info dict of the student
        """
        url = settings.HOST + "card/"
        rsp = requests.get(url, params={"student_id": student_id, "jwt": self.token}, verify=False)
        logging.debug("Response of {method} {url}: {rsp}".format(method="GET", url=url, rsp=rsp.text))
        return rsp.json()["results"][0]

    def increase_money(self, uid, money):
        url = settings.HOST + "card/increase_money/"
        rsp = requests.post(url, json={"card_id": uid, "money": money}, params={"jwt": self.token}, verify=False)
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        return rsp.json()

    def decrease_money(self, uid, money):
        url = settings.HOST + "card/decrease_money/"
        rsp = requests.post(url, json={"card_id": uid, "money": money}, params={"jwt": self.token}, verify=False)
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        return rsp.json()

    def put_card(self, uid: str, url: str = "", data: dict=None):
        if url == "":
            url = self.query_card_by_uid(uid)["url"]
        rsp = requests.put(url, data=data, params={"jwt": self.token}, verify=False)
        logging.debug("Response of {method} {url} {data}: {rsp}".format(method="PUT", url=url, rsp=rsp.text, data=data))
        return rsp.json()

    def delete_card(self, uid: str, url: str = ""):
        if url == "":
            url = self.query_card_by_uid(uid)["url"]
        payload = {"card_id": None}
        rsp = requests.put(url, data=json.dumps(payload), params={"jwt": self.token}, headers={'content-type': 'application/json'}, verify=False)
        logging.debug("Response of {method} {url} : {rsp}".format(method="PUT", url=url, rsp=rsp.text))
        return rsp.json()

    def create_card(self, student_id: str, uid: str):
        url = self.query_card_by_student_id(student_id)["url"]
        rsp = requests.put(url, data={"card_id": uid}, params={"jwt": self.token}, verify=False)
        logging.debug("Response of {method} {url} : {rsp}".format(method="PUT", url=url, rsp=rsp.text))
        return rsp.json()


if __name__ == '__main__':
    data_session = DataSession()
    print(data_session.authentication("root", "123xuimiao"))
    print(data_session.authentication("root", "123xuyimiao"))
    print(data_session.access("12345678"))
    print(data_session.access("123456789"))
    print(data_session.access("0123456789"))



