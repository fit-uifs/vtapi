import rpcz
import vtserver_interface_pb2
import vtserver_interface_rpcz

datasets = []

class VTServerInterface(vtserver_interface_rpcz.VTServerInterface):
    
    def addDataset(self, request, reply):
        print "addDataset: '%s'" % request.name
        response = vtserver_interface_pb2.addDatasetResponse()
        response.res.success = True
        datasets.append(request.name)
        response.dataset_id = request.name
        reply.send(response)
        
    def getDatasetList(self, request, reply):
        print "getDatasetList"
        response = vtserver_interface_pb2.getDatasetListResponse()
        response.res.success = True
        
        for ds in datasets:
        
            nds = response.datasets.add()
            nds.dataset_id = ds
            nds.name = ds
            
        reply.send(response)
                
    def addVideo(self, request, reply):
        print "addVideo: '%s'" % request.filepath
        response = vtserver_interface_pb2.addVideoResponse()
        response.res.success = True
        response.video_id = "video"
        reply.send(response)
              
    def getEventList(self, request, reply):
        print "getEventList"
        response = vtserver_interface_pb2.getEventListResponse()
        response.res.success = True
        
        ev1 = response.events_list.add()
        ev1.video_id = "video"
        ev11 = ev1.events.add()
        ev11.t1_sec = 10
        ev11.t2_sec = 20
        ev11.group_id = 123
        ev11.score = 0.8
        
        ev12 = ev1.events.add()
        ev12.t1_sec = 25
        ev12.t2_sec = 35
        ev12.group_id = 123
        ev12.score = 0.9
        
        ev13 = ev1.events.add()
        ev13.t1_sec = 40
        ev13.t2_sec = 50
        ev13.group_id = 123
        ev13.score = 0.85
        
        reply.send(response)
    
    def getDatasetList(self, request, reply):
        print "getDatasetList"
        response = vtserver_interface_pb2.getDatasetListResponse()
        response.res.success = True
        
        for ds in datasets:
        
            nds = response.datasets.add()
            nds.dataset_id = ds
            nds.name = ds
            
        reply.send(response)


if __name__ == "__main__":
  
    app = rpcz.Application()
    server = rpcz.Server(app)
    server.register_service(VTServerInterface(), "VTServerInterface")
    server.bind("tcp://*:5555")
    print "Serving requests on port 5555"
    app.run()