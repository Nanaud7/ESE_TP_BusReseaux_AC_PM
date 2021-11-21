from flask import Flask, jsonify, render_template, abort, request
import json
app = Flask(__name__)

temp = [0,0,32,0]
pres = [0,0,0,0]


@app.route('/temp/', methods=['POST'])
def temp_post(path = '/temp/'):
        resp = {
               "method": request.method,
               "url" :  request.url,
               "path" : path,
               "args": request.args,
               "headers": dict(request.headers),
        }
        if request.method == 'POST':
                resp["POST"] = {
                        "data" : request.get_json(),
                }
                json = request.get_json()
                temp.append(json["temp"])
        return jsonify(resp)

@app.route('/pres/', methods=['POST'])
def pres_post(path = '/pres/'):
        resp = {
               "method": request.method,
               "url" :  request.url,
               "path" : path,
               "args": request.args,
               "headers": dict(request.headers),
        }
        if request.method == 'POST':
                resp["POST"] = {
                        "data" : request.get_json(),
                }
                json = request.get_json()
                pres.append(json["pres"])
        return jsonify(resp)

@app.route('/temp/', methods=['GET'])
def temp_get():
        list = []
        for index in range(len(temp)):
                list.append({"index":index, "val": temp[index]})
        total = {"all": list}
        return jsonify(total)

@app.route('/pres/', methods=['GET'])
def pres_get():
        list = []
        for index in range(len(pres)):
                list.append({"index":index, "val": pres[index]})
        total = {"all": list}
        return jsonify(total)

@app.route('/temp/<int:index>', methods=['GET'])
def temp_get_index(index):
        if index > len(temp):
                abort(404)
        else:
                return jsonify({"index":index, "val": temp[index]})

@app.route('/pres/<int:index>', methods=['GET'])
def pres_get_index(index):
        if index > len(pres):
                abort(404)
        else:
                return jsonify({"index":index, "val": pres[index]})

@app.route('/temp/<int:index>', methods=['DELETE'])
def temp_delete_index(index, path = '/temp/<int:index>'):
        if index > len(temp):
                abort(404)
        else:
                temp.pop(index)
        resp = {
               "method": request.method,
               "url" :  request.url,
               "path" : path,
               "args": request.args,
               "headers": dict(request.headers),
        }

        return resp

@app.route('/pres/<int:index>', methods=['DELETE'])
def pres_delete_index(index, path = '/pres/<int:index>'):
        if index > len(pres):
                abort(404)
        else:
                pres.pop(index)
        resp = {
               "method": request.method,
               "url" :  request.url,
               "path" : path,
               "args": request.args,
               "headers": dict(request.headers),
        }

        return resp