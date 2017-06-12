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

if __name__ == "__main__":

    cl = VTServerClient("tcp://127.0.0.1:8719")

#################################################################
# BASIC COMMANDS USING DEMO DATASET
#
# before trying this, create demo dataset with:
# vtapi/sql/postgresql/demo-schema.sql

    # print 'datasets:' 
    # print cl.call('getDatasetList', {})
    # print '--------------'
    # print 'DEMO dataset'
    # print '--------------'
    # print 'metrics:'
    # print cl.call('getDatasetMetrics', {'dataset_id':'demo'})
    # print 'videos:'
    # print cl.call('getVideoIDList', {'dataset_id':'demo'})
    # print 'tasks:'
    # print cl.call('getTaskIDList', {'dataset_id':'demo'})
    # print 'processes:'
    # print cl.call('getProcessIDList', {'dataset_id':'demo'})
    # print 'some events:'
    # print cl.call('getEventList', {'dataset_id':'demo', 'video_ids':['video1','video2'], 'task_id':'task_demo2_1', 'filter': [{'min_duration': 0.1, 'max_duration': 12.0}]})
    # print '--------------'
    # print 'video info:'
    # print cl.call('getVideoInfo', {'dataset_id':'demo', 'video_ids':['video1','video2']})
    # print cl.call('setVideoInfo', {'dataset_id':'demo', 'video_id':'video1', 'start_time':{'seconds':1427861107,'nanos':0}})
    # print '--------------'
    # print 'task info:'
    # print cl.call('getTaskInfo', {'dataset_id':'demo', 'task_id':['task_demo1_1','task_demo1_2','task_demo2_1']})
    # print '--------------'


#################################################################
# FULL EXAMPLE
#
# 1. creates all objects from scratch into new dataset
# 2. tests VideoProcessing and EventDetection tasks and prints output events
# 3. deletes everything
#
# note:
#   tested module must be created in database (pg_modules.sql)


    test_dataset = 'test_client_py'
    test_video_path = '/home/vidte/vid.mp4'

    ################################################
    # SUMMARIZER TEST VALUES
    #
    # module = 'summarizer'
    # module_vp_params = [
    #     {'type':'TP_INT', 'name':'keyframe_freq' , 'value_int':25}]
    # module_ed_params = [
    #     {'type':'TP_FLOAT', 'name':'seg_length' , 'value_float':1.2},
    #     {'type':'TP_INT', 'name':'seg_count' , 'value_int':10},
    #     {'type':'TP_INT', 'name':'feature_id' , 'value_int':33}]
    #
    ################################################
    # PEOPLE TEST VALUES
    #
    module = 'videotype'
    module_vp_params = [
    {'type':'TP_INT', 'name':'keyframe_freq' , 'value_int':25}]
    module_ed_params = [
    {'type':'TP_FLOAT', 'name':'sensitivity' , 'value_float':0.5},
    {'type':'TP_INTARRAY', 'name':'search_classes' , 'value_int_array': [0,1,2,3,4]}    ]
    #
    ################################################

    ################################################
    # FACE TEST VALUES
    #
    # module = 'face'
    # module_vp_params = [
    #     {'type':'TP_FLOAT', 'name':'input_frame_scale' , 'value_float':0.5},
    #     {'type':'TP_INT', 'name':'max_size_x' , 'value_int':500},
    #     {'type':'TP_INT', 'name':'max_size_y' , 'value_int':500},
    #     {'type':'TP_INT', 'name':'min_size_x' , 'value_int':1},
    #     {'type':'TP_INT', 'name':'min_size_y' , 'value_int':1}]
    # module_ed_params = [
    #     {'type':'TP_INT', 'name':'minimum_duration' , 'value_int':2}]
    #
    ################################################


    ################################################
    # BEGIN
    ################################################

    print 'TEMPORARY dataset : ' + test_dataset
    print '--------------'

    print 'add dataset:'
    addDataset = cl.call('addDataset', {'name': test_dataset})
    print addDataset
    print '--------------'

    print 'get dataset list:'
    getDatasetList = cl.call('getDatasetList', {})
    print getDatasetList
    print '--------------'

    print 'add video:'
    addVideo = cl.call('addVideo', {
        'dataset_id': addDataset['dataset_id'],
        # 'name':'video32',                 # optional suggested name
        # 'location':'subdir/video32.mpg'   # optional suggested location in dataset
        'filepath': test_video_path,
        'start_time': {'seconds': 1439734399, 'nanos': 0}})
    print addVideo
    print '--------------'

    print 'get video info:'
    getVideoInfo = cl.call('getVideoInfo', {
        'dataset_id': addDataset['dataset_id'],
        'video_ids':[addVideo['video_id']]})
    print getVideoInfo
    print '--------------'

    print 'add task VIDEO PROCESSING:'
    addTask1 = cl.call('addTaskVideoProcessing', {
        'dataset_id': addDataset['dataset_id'],
        'module':module,
        'params':module_vp_params})
    print addTask1
    print '--------------'

    if addVideo['res']['success'] and addTask1['res']['success']:
        print 'run process VIDEO PROCESSING:'
        runProcess1 = cl.call('runProcess', {
            'dataset_id': addDataset['dataset_id'],
            'video_ids': [addVideo['video_id']],
            'task_id':addTask1['task_id']})
        print runProcess1
        print '--------------'

        print 'get task/process progress VIDEO PROCESSING:'
        p1_finished = False
        while not p1_finished:
            time.sleep(1)
            getProcessInfo1 = cl.call('getProcessInfo', {
                'dataset_id': addDataset['dataset_id'],
                'process_ids': [runProcess1['process_id']]})
            print getProcessInfo1
            getTaskProgress1 = cl.call('getTaskProgress', {
                'dataset_id':addDataset['dataset_id'],
                'task_id':addTask1['task_id'],
                'video_ids':[addVideo['video_id']]})
            print getTaskProgress1
            if getTaskProgress1['res']['success']:
                progress = getTaskProgress1['task_progress']['progress']
                p1_finished = (progress >= 1.0)
                print p1_finished
                # if progress >= 0.5:
                #     stopProcess1 = cl.call('stopProcess', {
                #         'dataset_id':addDataset['dataset_id'],
                #         'process_id':runProcess1['process_id']})
                #     print stopProcess1
                #     break
        print '--------------'

        print 'get task info VIDEO PROCESSING:'
        getTaskInfo1 = cl.call('getTaskInfo', {
            'dataset_id':addDataset['dataset_id'],
            'task_ids':[addTask1['task_id']]})
        print getTaskInfo1
        print '--------------'

    if addTask1['res']['success']:
        print 'add task EVENT DETECTION:'
        addTask2 = cl.call('addTaskEventDetection', {
            'dataset_id': addDataset['dataset_id'],
            'module':module,
            'prereq_task_id':addTask1['task_id'],
            'params':module_ed_params})
        print addTask2
        print '--------------'

        if addVideo['res']['success'] and addTask2['res']['success']:
            print 'run process EVENT DETECTION:'
            runProcess2 = cl.call('runProcess', {
                'dataset_id': addDataset['dataset_id'],
                'video_ids': [addVideo['video_id']],
                'task_id':addTask2['task_id']})
            print runProcess2
            print '--------------'

            print 'get task/process progress EVENT DETECTION:'
            p2_finished = False
            while not p2_finished:
                time.sleep(1)
                getProcessInfo2 = cl.call('getProcessInfo', {
                    'dataset_id': addDataset['dataset_id'],
                    'process_ids': [runProcess2['process_id']]})
                print getProcessInfo2
                getTaskProgress2 = cl.call('getTaskProgress', {
                    'dataset_id':addDataset['dataset_id'],
                    'task_id':addTask2['task_id'],
                    'video_ids':[addVideo['video_id']]})
                print "getTaskProgress2"
                print getTaskProgress2
                if getTaskProgress2['res']['success']:
                    progress = getTaskProgress2['task_progress']['progress']
                    p2_finished = (progress >= 1.0)
            print '--------------'

            print 'get task info EVENT DETECTION:'
            getTaskInfo2 = cl.call('getTaskInfo', {
                'dataset_id':addDataset['dataset_id'],
                'task_ids':[addTask2['task_id']]})
            print getTaskInfo2
            print '--------------'

            print 'get events:'
            getEventList = cl.call('getEventList', {
                'dataset_id':addDataset['dataset_id'],
                'task_id':addTask2['task_id'],
                'video_ids':[addVideo['video_id']]
                #,'filter':{'min_duration':1.8}
                #,'filter':{'begin_timewindow':{'seconds':1427861116,'nanos':0}}
                #,'filter':{'region':{'x1':0,'y1':0,'x2':0.0,'y2':0.0}}
                })
            print getEventList
            print '--------------'
            #sys.exit(0)

            print 'get event stats:'
            getEventList = cl.call('getEventsStats', {
                'dataset_id':addDataset['dataset_id'],
                'task_id':addTask2['task_id'],
                'video_ids':[addVideo['video_id']]})
            print getEventList
            print '--------------'

        if addTask2['res']['success']:
            print 'delete task EVENT DETECTION:'
            deleteTask2 = cl.call('deleteTask', {
                'dataset_id': addDataset['dataset_id'],
                'task_id':addTask2['task_id']})
            print deleteTask2
            print '--------------'

    if addTask1['res']['success']:
        print 'delete task VIDEO PROCESSING:'
        deleteTask1 = cl.call('deleteTask', {
            'dataset_id': addDataset['dataset_id'],
            'task_id':addTask1['task_id']})
        print deleteTask1
        print '--------------'

    if addVideo['res']['success']:
        print 'delete video:'
        print cl.call('deleteVideo', {
            'dataset_id': addDataset['dataset_id'],
            'video_id': addVideo['video_id']})
        print '--------------'

    print 'delete dataset:'
    print cl.call('deleteDataset', {'dataset_id': addDataset['dataset_id']})
