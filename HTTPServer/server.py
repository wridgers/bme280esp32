
import logging

from pyramid.config import Configurator
from pyramid.response import Response
from pyramid.view import view_config
from wsgiref.simple_server import make_server

LOGGER = logging.getLogger(__name__)


@view_config(request_method='POST', route_name='index', renderer='string')
def index(request):
    LOGGER.info('%r', request.json_body)
    return 'OK'


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)

    with Configurator() as config:
        config.add_route('index', '/')
        config.scan()

        app = config.make_wsgi_app()

    server = make_server('0.0.0.0', 8080, app)
    server.serve_forever()
