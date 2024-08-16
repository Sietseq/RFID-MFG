import requests

def pretty_print_POST(req):
    """
    At this point it is completely built and ready
    to be fired; it is "prepared".

    However pay attention at the formatting used in 
    this function because it is programmed to be pretty 
    printed and may differ from the actual request.
    """
    print('{}\n{}\r\n{}\r\n\r\n{}'.format(
        '-----------START-----------',
        req.method + ' ' + req.url,
        '\r\n'.join('{}: {}'.format(k, v) for k, v in req.headers.items()),
        req.body,
    ))

r=requests.Request("GET", "https://mackerel-star-civet.ngrok-free.app/koa-access-control-gate-cards/gate/asdsadasd", headers={"Authorization": "Basic db383e06709d9c823e72be8e7520e3bb"})
prepared = r.prepare()

pretty_print_POST(prepared)