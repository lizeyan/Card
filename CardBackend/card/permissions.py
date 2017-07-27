from rest_framework import permissions


class OnlySuperUserPermission(permissions.BasePermission):
    """
    Allows access only to authenticated users.
    """

    def has_permission(self, request, view):
        return request.user.is_superuser == 1


class IncreasePermission(permissions.BasePermission):
    def has_permission(self, request, view):
        return request.user.is_superuser == 1 or request.user.profile.user_type == 'atm'


class AccessPermission(permissions.BasePermission):
    def has_permission(self, request, view):
        return request.user.is_superuser == 1 or request.user.profile.user_type == 'access'


class DecreasePermission(permissions.BasePermission):
    def has_permission(self, request, view):
        return request.user.is_superuser == 1 or request.user.profile.user_type == 'pos' \
               or request.user.profile.user_type == 'atm'


class RegisterPermission(permissions.BasePermission):
    def has_permission(self, request, view):
        return request.user.is_superuser == 1 or request.user.profile.user_type == 'register' \
               or request.user.profile.user_type == 'pos' or request.user.profile.user_type == 'atm'

