import time
import sys
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

    def call(self, service, req):
        
        try:    
            request = getattr(vtserver_interface_pb2, service + "Request")()
        except AttributeError:
            raise VTServerException('Service "' + service + '" does not exist.')

            
        srv_method = getattr(self.srv, service)
        
        try:
            response = srv_method(to_protobuf(request, req), deadline_ms=self.deadline_ms)
        except AttributeError:
            raise VTServerException('Request must be a dictionary.')
        except EncodeError:
            raise VTServerException('Request is not correct (some required field is missing?).')
        except RpcDeadlineExceeded:
            return {'res': {'success': False, 'error': 'No answer from server in given time.'}}
            #raise VTServerException('No answer from server in given time.')

        
        return to_dict(response)

if __name__ == "__main__":

    cl = VTServerClient("tcp://127.0.0.1:8719")

    test_dataset = 'test_client_py'
    test_video_path = '/home/vidte/vid.mp4'
    #test_video_path = '/home/vojca/people.mp4'
    test_video_name = 'vt_test_vid'

    module = 'videotype'
    module_vp_params = [
    {'type':'TP_INT', 'name':'keyframe_freq' , 'value_int':25}]
    module_ed_params = [
    {'type':'TP_FLOAT', 'name':'sensitivity' , 'value_float':0.5}, {'type':'TP_INTARRAY', 'name': 'search_classes', 'value_int_array': [1,2,3,4,5,6,7,8,203]}]


########## PREPARATION

    print module_vp_params
    print 'TEMPORARY dataset : ' + test_dataset
    print '--------------'

    print 'add dataset:'
    addDataset = cl.call('addDataset', {'name': test_dataset})
    print addDataset
    print '--------------'

    print 'check video:'
    getVideoList = cl.call('getVideoIDList', {
        'dataset_id': addDataset['dataset_id']})
    print getVideoList

    if 'video_ids' in getVideoList and test_video_name in getVideoList['video_ids']:
        print 'exists'
    else:
        print 'add video:'
        addVideo = cl.call('addVideo', {
            'dataset_id': addDataset['dataset_id'],
            'name':test_video_name,
            'filepath': test_video_path,
            'start_time': {'seconds': 1439734399, 'nanos': 0}})
        print addVideo
    print '--------------'


########## VIDEO PROCESSING


    print 'add task VIDEO PROCESSING:'
    addTaskVP = cl.call('addTaskVideoProcessing', {
        'dataset_id': addDataset['dataset_id'],
        'module':module,
        'params':module_vp_params})
    print addTaskVP
    print '--------------'

    if addTaskVP['res']['success']:
        print 'run process VIDEO PROCESSING:'
        runProcess1 = cl.call('runProcess', {
            'dataset_id': addDataset['dataset_id'],
            'video_ids': [test_video_name],
            'task_id':addTaskVP['task_id']})
        print runProcess1
        print '--------------'

        p1_finished = False
        while not p1_finished:
            time.sleep(1)
            getTaskProgress1 = cl.call('getTaskProgress', {
                'dataset_id':addDataset['dataset_id'],
                'task_id':addTaskVP['task_id'],
                'video_ids':[test_video_name]})
            if getTaskProgress1['res']['success']:
                progress = getTaskProgress1['task_progress']['progress']
                p1_finished = (progress >= 1.0)

        print 'get task info VIDEO PROCESSING:'
        getTaskInfo1 = cl.call('getTaskInfo', {
            'dataset_id':addDataset['dataset_id'],
            'task_ids':[addTaskVP['task_id']]})
        print getTaskInfo1
        print '--------------'
        

########## PROCESSING METADATA


    if addTaskVP['res']['success']:
        print 'add task PROCESSING METADATA:'
        time.sleep(3) # TEMPORARY UNTIL FIX
        addTaskPM = cl.call('addTaskProcessingMetadata', {
            'dataset_id': addDataset['dataset_id'],
            'module':module,
            'prereq_task_id':addTaskVP['task_id'],
            'params': []
        })
        print addTaskPM
        print '--------------'
        
        print 'run process PROCESSING METADATA:'
        runProcessPM = cl.call('runProcess', {
                    'dataset_id': addDataset['dataset_id'],
                    'video_ids': [test_video_name],
                    'task_id':addTaskPM['task_id']})
        print runProcessPM
        print '--------------'
        
        pmd_finished = False
        while not pmd_finished:
            time.sleep(1)
            getTaskProgressPM = cl.call('getTaskProgress', {
                'dataset_id':addDataset['dataset_id'],
                'task_id': addTaskPM['task_id'],
                'video_ids':[test_video_name]})
            if getTaskProgressPM['res']['success']:
                progress = getTaskProgressPM['task_progress']['progress']
                pmd_finished = (progress >= 1.0)

        print 'get task info PROCESSING METADATA:'
        getTaskInfoPM = cl.call('getTaskInfo', {
            'dataset_id':addDataset['dataset_id'],
            'task_ids':[addTaskPM['task_id']]})
        print getTaskInfoPM
        print '--------------'

        print 'get processing metadata:'
        getProcessingMetadata = cl.call('getProcessingMetadata', {
            'dataset_id':addDataset['dataset_id'],
            'task_id': addTaskPM['task_id'],
            'video_ids': [test_video_name],
            
        })
        print getProcessingMetadata
        print '--------------'   


########## EVENT DETECTION


        if addTaskPM['res']['success']:
            print 'add task EVENT DETECTION:'
            addTaskED = cl.call('addTaskEventDetection', {
                'dataset_id': addDataset['dataset_id'],
                'module':module,
                'prereq_task_id':addTaskVP['task_id'],
                'params':module_ed_params})
            print addTaskED
            print '--------------'

            if addTaskED['res']['success']:
                print 'run process EVENT DETECTION:'
                runProcess2 = cl.call('runProcess', {
                    'dataset_id': addDataset['dataset_id'],
                    'video_ids': [test_video_name],
                    'task_id':addTaskED['task_id']})
                print runProcess2
                print '--------------'

                print 'get task/process progress EVENT DETECTION:'
                p2_finished = False
                while not p2_finished:
                    time.sleep(1)
                    getTaskProgress2 = cl.call('getTaskProgress', {
                        'dataset_id':addDataset['dataset_id'],
                        'task_id':addTaskED['task_id'],
                        'video_ids':[test_video_name]})
                    if getTaskProgress2['res']['success']:
                        progress = getTaskProgress2['task_progress']['progress']
                        p2_finished = (progress >= 1.0)

                print 'get task info EVENT DETECTION:'
                getTaskInfo2 = cl.call('getTaskInfo', {
                    'dataset_id':addDataset['dataset_id'],
                    'task_ids':[addTaskED['task_id']]})
                print getTaskInfo2
                print '--------------'

                print 'get events:'
                getEventList = cl.call('getEventList', {
                    'dataset_id':addDataset['dataset_id'],
                    'task_id':addTaskED['task_id'],
                    'video_ids':[test_video_name]
                    # ,'filter':{'min_duration':1.8}
                    #,'filter':{'begin_timewindow':{'seconds':1427861116,'nanos':0}}
                    })
                print getEventList
                print '--------------'

                print 'get event stats:'
                getEventList = cl.call('getEventsStats', {
                    'dataset_id':addDataset['dataset_id'],
                    'task_id':addTaskED['task_id'],
                    'video_ids':[test_video_name]})
                print getEventList
                print '--------------'

        #if addTaskED['res']['success']:
        #    print 'delete task EVENT DETECTION:'
        #    deleteTask2 = cl.call('deleteTask', {
        #        'dataset_id': addDataset['dataset_id'],
        #        'task_id':addTaskED['task_id']})
        #    print deleteTask2
        #    print '--------------'

    #if addTaskVP['res']['success']:
    #    print 'delete task VIDEO PROCESSING:'
    #    deleteTask1 = cl.call('deleteTask', {
    #        'dataset_id': addDataset['dataset_id'],
    #        'task_id':addTaskVP['task_id']})
    #    print deleteTask1
    #    print '--------------'

    #if addVideo['res']['success']:
    #    print 'delete video:'
    #    print cl.call('deleteVideo', {
    #        'dataset_id': addDataset['dataset_id'],
    #        'video_id': test_video_name})
    #    print '--------------'

    #print 'delete dataset:'
    #print cl.call('deleteDataset', {'dataset_id': addDataset['dataset_id']})
