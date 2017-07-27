from django.shortcuts import render
from django.contrib.auth.models import User
from django.views.decorators.csrf import csrf_exempt
from rest_framework.authentication import SessionAuthentication, BasicAuthentication
from rest_framework.decorators import api_view, authentication_classes, permission_classes
from rest_framework import permissions
from rest_framework import viewsets
from rest_framework.filters import DjangoFilterBackend
from rest_framework.parsers import JSONParser
from rest_framework.permissions import IsAuthenticated
from rest_framework_jwt.authentication import JSONWebTokenAuthentication
from card.models import Card, Profile, Log
from card.permissions import OnlySuperUserPermission, AccessPermission, DecreasePermission, IncreasePermission, \
    RegisterPermission
from card.serializers import UserSerializer, CardSerializer, LogSerializer
from django.db import IntegrityError
from card.interface import JSONResponse
from datetime import timedelta, datetime
from card.auth import JSONWebTokenAuthenticationQS
from rest_framework.response import Response
from rest_framework import status


class UserViewSet(viewsets.ReadOnlyModelViewSet):
    """
    """
    serializer_class = UserSerializer
    queryset = User.objects.all()
    permission_classes = (permissions.IsAuthenticated,)


class LogViewSet(viewsets.ReadOnlyModelViewSet):
    """
    """
    serializer_class = LogSerializer
    queryset = Log.objects.all()
    permission_classes = (permissions.IsAuthenticated, OnlySuperUserPermission)


class CardViewSet(viewsets.ModelViewSet):
    """
    add `card_id` params to filter card that you want
    """
    serializer_class = CardSerializer
    permission_classes = (permissions.IsAuthenticated, RegisterPermission)
    queryset = Card.objects.all()
    filter_backends = (DjangoFilterBackend,)
    filter_fields = ('card_id', 'student_id', )

    def destroy(self, request, *args, **kwargs):
        if request.user.profile.user_type == 'pos' or request.user.profile.user_type == 'atm':
            return JSONResponse({'status': 'forbidden'}, status=404)
        instance = self.get_object()
        self.perform_destroy(instance)
        return Response(status=status.HTTP_204_NO_CONTENT)

    def update(self, request, *args, **kwargs):
        if request.user.profile.user_type == 'pos' or request.user.profile.user_type == 'atm':
            return JSONResponse({'status': 'forbidden'}, status=404)
        partial = kwargs.pop('partial', False)
        instance = self.get_object()
        serializer = self.get_serializer(instance, data=request.data, partial=partial)
        serializer.is_valid(raise_exception=True)
        self.perform_update(serializer)

        if getattr(instance, '_prefetched_objects_cache', None):
            # If 'prefetch_related' has been applied to a queryset, we need to
            # forcibly invalidate the prefetch cache on the instance.
            instance._prefetched_objects_cache = {}

        return Response(serializer.data)

    def retrieve(self, request, *args, **kwargs):
        if request.user.profile.user_type == 'pos' or request.user.profile.user_type == 'atm':
            return JSONResponse({'status': 'forbidden'}, status=404)
        instance = self.get_object()
        serializer = self.get_serializer(instance)
        return Response(serializer.data)

    def create(self, request, *args, **kwargs):
        if request.user.profile.user_type == 'pos' or request.user.profile.user_type == 'atm':
            return JSONResponse({'status': 'forbidden'}, status=404)
        serializer = self.get_serializer(data=request.data)
        serializer.is_valid(raise_exception=True)
        self.perform_create(serializer)
        headers = self.get_success_headers(serializer.data)
        return Response(serializer.data, status=status.HTTP_201_CREATED, headers=headers)



@csrf_exempt
@api_view(['POST'])
@authentication_classes((SessionAuthentication, BasicAuthentication, JSONWebTokenAuthentication, JSONWebTokenAuthenticationQS))
@permission_classes((IsAuthenticated, OnlySuperUserPermission,))
def do_register(request):
    """
    `user_type` can be `access`, `pos`, `atm`, `register`
    """
    data = JSONParser().parse(request)
    new_account = {}
    user_type_list = ['access', 'pos', 'atm', 'register']
    try:
        username = data['username']
        password = data['password']
        user_type = data['user_type']
        new_account['user_name'] = username
        new_account['password'] = password
        new_account['user_type'] = user_type
        if user_type not in user_type_list:
            responseMess = {'status': 'NO_THIS_USER_TYPE'}
            return JSONResponse(responseMess, status=400)
    except:
        responseMess = {'status': 'INPUT_STYLE_ERROR'}
        return JSONResponse(responseMess, status=400)
    try:
        user = User.objects.create_user(
            username=new_account['user_name'], password=new_account['password'], email='')
        profile = Profile(user=user, user_type=new_account['user_type'])
        profile.save()
        info = 'created by ' + str(request.user.username)
        log = Log(action='do_register_user', username=username, info=info)
        log.save()
    except IntegrityError:
        responseMess = {'status': 'USERNAME_ALREADY_EXIST', 'suggestion': '用户名已经存在，请更换用户名'}
        return JSONResponse(responseMess, status=400)
    responseMess = {'status': 'CREATED'}
    return JSONResponse(responseMess, status=201)


@csrf_exempt
@api_view(['POST'])
@authentication_classes((SessionAuthentication, BasicAuthentication, JSONWebTokenAuthentication, JSONWebTokenAuthenticationQS))
@permission_classes((IsAuthenticated, DecreasePermission,))
def decrease_money(request):
    """
    :param request: `card_id`, `money`
    :return: `status`
    """
    data = JSONParser().parse(request)
    try:
        card_id = data['card_id']
        money = data['money']
        money = float(money)
        if money < 0:
            responseMess = {'status': 'MONEY_MUST_BE_POSITIVE'}
            return JSONResponse(responseMess, status=400)
        this_card_list = Card.objects.filter(card_id=card_id)
        if this_card_list.count() == 0:
            responseMess = {'status': 'NO_THIS_CARD'}
            return JSONResponse(responseMess, status=400)
        this_card = this_card_list.get(card_id=card_id)
        if this_card.money < money:
            responseMess = {'status': 'MONEY_IS_NOT_ENOUGH'}
            return JSONResponse(responseMess, status=400)
        new_money = this_card.money - money
        this_card.money = new_money
        this_card.save()
        info = 'decrease money ' + str(money) + ', final money is ' + str(new_money) + '.'
        log = Log(action='decrease_money', username=request.user.username, card_id=card_id, info=info)
        log.save()
        responseMess = {'status': 'success', 'now_money': new_money}
        return JSONResponse(responseMess, status=200)
    except:
        responseMess = {'status': 'INPUT_STYLE_ERROR'}
        return JSONResponse(responseMess, status=400)


@csrf_exempt
@api_view(['POST'])
@authentication_classes((SessionAuthentication, BasicAuthentication, JSONWebTokenAuthentication, JSONWebTokenAuthenticationQS))
@permission_classes((IsAuthenticated, IncreasePermission,))
def increase_money(request):
    """
    :param request: `card_id`, `money`
    :return: `status`
    """
    data = JSONParser().parse(request)
    try:
        card_id = data['card_id']
        money = data['money']
        money = float(money)
        if money < 0:
            responseMess = {'status': 'MONEY_MUST_BE_POSITIVE'}
            return JSONResponse(responseMess, status=400)
        this_card_list = Card.objects.filter(card_id=card_id)
        if this_card_list.count() == 0:
            responseMess = {'status': 'NO_THIS_CARD'}
            return JSONResponse(responseMess, status=400)
        this_card = this_card_list.get(card_id=card_id)
        new_money = this_card.money + money
        this_card.money = new_money
        this_card.save()
        info = 'increase money ' + str(money) + ', final money is ' + str(new_money) + '.'
        log = Log(action='increase_money', username=request.user.username, card_id=card_id, info=info)
        log.save()
        responseMess = {'status': 'success', 'now_money': new_money}
        return SONResponse(responseMess, status=200)
    except:
        responseMess = {'status': 'INPUT_STYLE_ERROR'}
        return JSONResponse(responseMess, status=400)


@csrf_exempt
@api_view(['POST'])
@authentication_classes((SessionAuthentication, BasicAuthentication, JSONWebTokenAuthentication, JSONWebTokenAuthenticationQS))
@permission_classes((IsAuthenticated, AccessPermission,))
def access(request):
    """
    :param request: `card_id`
    :return: `status`
    """
    data = JSONParser().parse(request)
    try:
        card_id = data['card_id']
        this_card_list = Card.objects.filter(card_id=card_id)
        if this_card_list.count() == 0:
            responseMess = {'status': 'NO_THIS_CARD'}
            return JSONResponse(responseMess, status=400)
        time = datetime.now()
        this_card_list = Card.objects.filter(card_id=card_id, begin_time__lt=time, end_time__gt=time)
        if this_card_list.count() == 0:
            responseMess = {'status': 'FORBIDDEN'}
            return JSONResponse(responseMess, status=400)
        else:
            log = Log(action='access', username=request.user.username, card_id=card_id)
            log.save()
            responseMess = {'status': 'ACCESS'}
            return JSONResponse(responseMess, status=200)
    except:
        responseMess = {'status': 'INPUT_STYLE_ERROR'}
        return JSONResponse(responseMess, status=400)
