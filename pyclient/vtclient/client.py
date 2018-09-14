import rpcz
import vtserver_interface_pb2
import vtserver_interface_rpcz
from protodict import to_dict, to_protobuf
from google.protobuf.message import EncodeError
from rpcz.rpc import RpcDeadlineExceeded

class VTServerException(Exception):
    pass

class VTServerClient(object):

    def __init__(self, conn_str, deadline_ms=5000):

        self.conn_str = conn_str
        self.deadline_ms = deadline_ms
        self.app = rpcz.Application()
        self.srv = vtserver_interface_rpcz.VTServerInterface_Stub(self.app.create_rpc_channel(self.conn_str))

    def call(self, service, req, deadline_ms = None):
        if deadline_ms is None:
            deadline_ms = self.deadline_ms

        try:
            request = getattr(vtserver_interface_pb2, service + "Request")()
        except AttributeError:
            raise VTServerException('Service "' + service + '" does not exist.')


        srv_method = getattr(self.srv, service)

        try:
            response = srv_method(to_protobuf(request, req), deadline_ms = deadline_ms)
        except AttributeError:
            print "[Client error]: Request must be a dictionary."
            raise VTServerException('Request must be a dictionary.')
        except EncodeError:
            print "[Client error]: Request is not correct (some required field is missing?)."
            raise VTServerException('Request is not correct (some required field is missing?).')
        except RpcDeadlineExceeded:
            print "[Client error]: No answer from server in given time."
            return {'res': {'success': False, 'error': 'No answer from server in given time.'}}
            #raise VTServerException('No answer from server in given time.')


        return to_dict(response)
