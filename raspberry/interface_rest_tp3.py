from flask import Flask, jsonify, render_template, abort, request
import json
app = Flask(__name__)

@app.route('/')
def hello_world():
        return 'Hello, World!\n'

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/', methods=['GET'])
def api_welcome():
        return welcome

@app.route('/api/welcome/', methods=['POST'])
def api_welcome_post(path = '/api/welcome/'):
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
        return jsonify(resp)


@app.route('/api/welcome/<int:index>', methods=['GET'])
def api_welcome_index(index):
        if index > len(welcome):
                abort(404)
        else:
                return jsonify({"index": index, "val": welcome[index]})

@app.route('/api/welcome/<int:index>', methods=['PUT'])
def api_welcome_index_put(index):
        global welcome
        data = request.get_json()
        if index > len(welcome):
                abort(404)
        else:

                welcome = welcome[:index]+data+welcome[index:]
                return welcome

@app.route('/api/welcome/<int:index>', methods=['PATCH'])
def api_welcome_index_patch(index):
        data = request.get_json()
        if index > len(welcome):
                abort(404)
        else:
                return welcome[:index]+data+welcome[index:]


@app.route('/api/welcome/<int:index>', methods=['DELETE'])
def api_welcome_index_delete_x(index):
        if index > len(welcome):
                abort(404)
        else:
                return welcome[:index]+welcome[index:]

@app.route('/api/welcome/', methods=['DELETE'])
def api_welcome_index_delete(index):
        data = request.get_json()
        if index > len(welcome):
 else:
                return ""

@app.errorhandler(404)
def page_not_found(error):
    return render_template('page_not_found.html'), 404

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
"args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                }
    return jsonify(resp)

