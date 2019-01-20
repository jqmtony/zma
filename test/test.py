import os
import json
import base64
import zkclient as zk


rootPath = "/test"
cronFiles = ['mo_load.sh']
zcli = zk.ZKClient( '127.0.0.1:2181', int(3000))
if not zcli.exists(rootPath):
    zcli.create( rootPath )


for cronFile in cronFiles:

    with open(cronFile, 'r') as f:
        fileContent = f.read()
    nodeValue = {
        'type': 'cron',
        'detail': {
            'name': cronFile.split('.')[0],
            'status': 1,
            'filename': cronFile,
            'interval': 10,
            'command': "sh %s" % cronFile,
            'fcontent': base64.b64encode(fileContent)
        }
    }
    
    path = os.path.join(rootPath, cronFile.split('.')[0] )
    if zcli.exists(path):
        print zcli.set(path, data=json.dumps(nodeValue))
    else:
        print zcli.create( path, data=json.dumps(nodeValue) )
        
zcli.close()
