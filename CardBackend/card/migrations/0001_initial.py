# -*- coding: utf-8 -*-
# Generated by Django 1.11.3 on 2017-07-26 18:21
from __future__ import unicode_literals

import datetime
from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Card',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('card_id', models.CharField(blank=True, default='', max_length=100, null=True, unique=True)),
                ('name', models.CharField(blank=True, default='', max_length=50)),
                ('student_id', models.CharField(blank=True, default='', max_length=20, unique=True)),
                ('begin_time', models.DateTimeField(blank=True, default=datetime.datetime.now)),
                ('end_time', models.DateTimeField(blank=True, default=datetime.datetime.now)),
                ('money', models.FloatField(blank=True, default=0)),
            ],
        ),
        migrations.CreateModel(
            name='Log',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('action', models.CharField(blank=True, default='', max_length=100)),
                ('time', models.DateTimeField(blank=True, default=datetime.datetime.now)),
                ('username', models.CharField(blank=True, default='', max_length=100)),
                ('card_id', models.CharField(blank=True, default='', max_length=100)),
                ('info', models.CharField(blank=True, default='', max_length=1000)),
            ],
        ),
        migrations.CreateModel(
            name='Profile',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('user_type', models.CharField(blank=True, default='', max_length=100)),
                ('user', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, related_name='profile', to=settings.AUTH_USER_MODEL)),
            ],
        ),
    ]
