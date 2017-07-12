from django.db import models
from django.contrib.auth.models import User
import datetime


# Create your models here.

class Card(models.Model):
    card_id = models.CharField(max_length=100, blank=True, default='', unique=True)
    name = models.CharField(max_length=50, blank=True, default='')
    student_id = models.CharField(max_length=20, blank=True, default='', unique=True)
    begin_time = models.DateTimeField(blank=True, default=datetime.datetime.now)
    end_time = models.DateTimeField(blank=True, default=datetime.datetime.now)
    money = models.FloatField(blank=True, default=0)


class Profile(models.Model):
    user_type = models.CharField(max_length=100, blank=True, default='')
    user = models.OneToOneField(User, related_name='profile', unique=True)


class Log(models.Model):
    action = models.CharField(max_length=100, blank=True, default='')
    time = models.DateTimeField(blank=True, default=datetime.datetime.now)
    username = models.CharField(max_length=100, blank=True, default='')
    card_id = models.CharField(max_length=100, blank=True, default='')
    info = models.CharField(max_length=1000, blank=True, default='')
