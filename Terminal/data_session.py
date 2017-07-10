import requests
import settings
import logging


class DataSession(object):
    def __init__(self):
        self.token = ""

    def authentication(self, user_name: str, password: str) -> dict:
        """
        :param user_name:
        :param password:
        :return: token. empty string if authentication failed.
        """
        url = settings.HOST + "api-token-auth/"
        rsp = requests.post(url, data={"username": user_name, "password": password})
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
        rsp = requests.post(url, json={"card_id": uid}, headers={"Authorization": "JWT " + self.token})
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        return rsp.status_code == 200, rsp.json()["status"]

    def query_card(self, uid: str) -> dict:
        """
        :param uid:
        :return: info dict of the student
        """
        url = settings.HOST + "card/"
        rsp = requests.get(url, params={"card_id": uid}, headers={"Authorization": "JWT " + self.token})
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        return rsp.json()["results"][0]

    def increase_money(self, uid, money):
        url = settings.HOST + "card/increase_money/"
        rsp = requests.post(url, params={"card_id": uid, "money": money}, headers={"Authorization": "JWT " + self.token})
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        return rsp.json()

    def decrease_money(self, uid, money):
        url = settings.HOST + "card/decrease_money/"
        rsp = requests.post(url, params={"card_id": uid, "money": money}, headers={"Authorization": "JWT " + self.token})
        logging.debug("Response of {method} {url}: {rsp}".format(method="POST", url=url, rsp=rsp.text))
        return rsp.json()

if __name__ == '__main__':
    data_session = DataSession()
    print(data_session.authentication("root", "123xuimiao"))
    print(data_session.authentication("root", "123xuyimiao"))
    print(data_session.access("12345678"))
    print(data_session.access("123456789"))
    print(data_session.access("0123456789"))



