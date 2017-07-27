#!/usr/bin/env bash

pyinstaller -F -w terminal.py --hidden-import=queue
