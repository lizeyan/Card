import logging
import sys


FORMAT = '%(asctime)-15s %(message)s'
logging.basicConfig(handlers=[logging.FileHandler("terminal.log", encoding="utf-8"), logging.StreamHandler(sys.stdout)], level=logging.DEBUG, format=FORMAT)
HOST = "https://card.lizeyan.me/"
KEY = "akxcuO9.2nJe!\"1Do\'#uNa.0r#lb8Ia4%FX=z@hlvEi2et-*ssf62t#047YO@@(qQ.!HEZ--~LiNSpSvhqeGcq-q#UjaI*aj6J'Rpv^xnSejjX%ih2gUZ(0JVOYqOf!~"
IV = "5413251766712441"
