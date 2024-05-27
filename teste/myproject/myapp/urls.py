# myapp/urls.py
from django.urls import path, include
from .views import TaskViewSet, index
from rest_framework.routers import DefaultRouter

router = DefaultRouter()
router.register(r'tasks', TaskViewSet)

urlpatterns = [
    path('', index, name='index'),
    path('api/', include(router.urls)),
]
