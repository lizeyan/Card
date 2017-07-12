from django.conf.urls import url, include
from card import views
from rest_framework.routers import DefaultRouter

router = DefaultRouter(schema_title='Pastebin API')
router.register(r'users', views.UserViewSet, base_name='user')
router.register(r'card', views.CardViewSet, base_name='card')
router.register(r'log', views.LogViewSet, base_name='log')

urlpatterns = [
    url(r'^decrease_money/$', views.decrease_money),
    url(r'^increase_money/$', views.increase_money),
    url(r'^access/$', views.access),
]

