from django.contrib.auth.models import User
from rest_framework import serializers
from card.models import Card, Log


class CardSerializer(serializers.ModelSerializer):
    card_money = serializers.ReadOnlyField(source='money')

    class Meta:
        model = Card
        fields = ('url', 'pk', 'card_id', 'name', 'student_id', 'begin_time', 'end_time', 'card_money',)


class UserSerializer(serializers.HyperlinkedModelSerializer):
    user_type = serializers.ReadOnlyField(source='profile.user_type')

    class Meta:
        model = User
        fields = ('url', 'pk', 'username', 'is_superuser', 'user_type')


class LogSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Log
        fields = ('url', 'pk', 'action', 'time', 'username', 'card_id', 'info')
