from django.shortcuts import render

def base(request):
    """
    BASE PARA TEMPLATES
    """
    return render(request, 'base.html')

def index(request):
    """
    Não necessita de uma verificação do tipo de requisição,
    pois retornaremos apenas um layout
    """
    return render(request,"routers/pages/index.html")
    
    
def about(request):
    """
    Não necessita de uma verificação do tipo de requisição,
    pois retornaremos apenas um layout
    """
    return render(request,"routers/pages/about.html")
    
def contact(request):
    """
    Não necessita de uma verificação do tipo de requisição,
    pois retornaremos apenas um layout,neste caso para enviar
    um formulário, deve-se criar outra view com um template próprio
    para seu formulário.
    """
    return render(request,"routers/pages/contact.html")
