from rest_framework_jwt.settings import api_settings
from rest_framework_jwt.authentication import BaseJSONWebTokenAuthentication

class JSONWebTokenAuthenticationQS(BaseJSONWebTokenAuthentication):
    def get_jwt_value(self, request):
        return request.query_params.get('jwt')
