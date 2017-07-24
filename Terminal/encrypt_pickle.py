import pickle
from Crypto.Cipher import AES
from Crypto.Util import Counter
import settings
import os


def encrypt(key, data: bytes):
    counter = Counter.new(128, initial_value=settings.IV.encode("utf-8"))
    cipher = AES.new(key, AES.MODE_CTR, counter=counter)
    encrypted = cipher.encrypt(data)
    return encrypted


def decrypt(key, data: bytes):
    counter = Counter.new(128, initial_value=settings.IV.encode("utf-8"))
    cipher = AES.new(key, AES.MODE_CTR, counter=counter)
    return cipher.decrypt(data)


def dump_encrypt(obj, filename: str):
    data = pickle.dumps(obj)
    encrypted = encrypt(settings.KEY, data)
    with open(filename, "wb") as f:
        f.write(encrypted)


def load_decrypt(filename):
    if not os.path.exists(filename) or not os.path.isfile(filename):
        return None
    with open(filename, "rb") as f:
        encrypted = f.read()
    data = decrypt(settings.KEY, encrypted)
    return pickle.loads(data)


def money2int(money):
    money_100 = int(money *100)
    money_str = str(money_100)
    money_str = '0' * (16 - len(money_str)) + money_str
    eptd = encrypt(settings.KEY[:32], money_str.encode("utf-8"))
    ans = [0, 0, 0, 0]
    for i in range(4):
        ans[i] = int(eptd[i * 4]) + 256 * int(eptd[i * 4 + 1]) + 256 * 256 * int(eptd[i * 4 + 2]) + \
                 (256 ** 3) * int(eptd[i * 4 + 3])
    return ans


def int2money(money_int):
    money_int = [int(x) for x in money_int]
    money16 = []
    for i in range(4):
        for k in range(4):
            money16.append(int(money_int[i]) % 256)
            money_int[i] /= 256
    money16 = [x.to_bytes(1, byteorder='big') for x in money16]
    ans = money16[0]
    for i in range(1, 16):
        ans = ans + money16[i]
    dptd = decrypt(settings.KEY[:32], ans)
    ans = float(int(dptd.decode('utf-8'))) / 100
    return ans

if __name__ == '__main__':
    ans = money2int(12312333)
    print(ans)
    money = int2money(ans)
    print(money)
