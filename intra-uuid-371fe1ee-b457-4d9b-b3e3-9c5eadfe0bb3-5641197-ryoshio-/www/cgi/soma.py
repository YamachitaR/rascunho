import os

def soma():
    for var, value in os.environ.items():
        if("QUERY_STRING" == var):
            list = [int(x) for x in value.split("&")]
            print("Sum =", sum(list))

if __name__ == '__main__':
    soma()