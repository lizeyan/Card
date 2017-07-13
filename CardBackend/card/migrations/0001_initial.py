# -*- coding: utf-8 -*-
# Generated by Django 1.10.1 on 2017-07-09 14:04
from __future__ import unicode_literals

import datetime
from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Card',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('card_id', models.CharField(blank=True, default='', max_length=100, unique=True)),
                ('name', models.CharField(blank=True, default='', max_length=50)),
                ('student_id', models.CharField(blank=True, default='', max_length=20, unique=True)),
                ('begin_time', models.DateTimeField(blank=True, default=datetime.datetime.now)),
                ('end_time', models.DateTimeField(blank=True, default=datetime.datetime.now)),
                ('money', models.FloatField(blank=True, default=0)),
            ],
        ),
    ]