import os
import datetime
import string

from flask import Flask
from flask import render_template
from flask import Blueprint

app = Flask(__name__)
iosig_data_path = os.getenv('IOSIG_DATA')
print 'root path: ', app.root_path
iosig_data = Blueprint('iosig_data', __name__, url_prefix='/iosig_data', static_folder=iosig_data_path)
app.register_blueprint(iosig_data)

@app.route('/')
@app.route('/iosig')
def list_trace_dirs():
    # list the $IOSIG_DATA
    #iosig_data_path = os.getenv('IOSIG_DATA')
    if (iosig_data_path == None):
        return render_template('trace_dirs.html', list=None)
    dirs = [ trace_dir for trace_dir in os.listdir(iosig_data_path) if os.path.isdir(os.path.join(iosig_data_path, trace_dir)) ]
    dirs.sort(reverse=True)
    dirs_list = []
    for directory in dirs:
        user, separator, the_rest = directory.partition('_')
        ts_epoch, separator, exe_name = the_rest.partition('_')
        date_string = datetime.datetime.fromtimestamp(float(ts_epoch)).strftime('%Y-%m-%d %H:%M:%S')
        dirs_list.append([user, date_string, exe_name, directory])

    return render_template('iosig_list.html', list=dirs_list)

@app.route('/details/<trace_id>')
def show_trace_details(trace_id):
    #iosig_data_path = os.getenv('IOSIG_DATA')
    trace_data_path = os.path.join(iosig_data_path, trace_id)
    analysis_result_path = os.path.join(trace_data_path, 'result_output')
    sub_path = os.path.join(trace_id, 'result_output')
    if os.path.exists(analysis_result_path) == False:
        return render_template('iosig_details.html', trace_id=None)

    #figures = [ os.path.join(sub_path, figure) for figure in os.listdir(analysis_result_path) if figure.endswith('png') ]
    #figures.sort()
    iorate_figures = [ os.path.join(sub_path, figure) for figure in os.listdir(analysis_result_path) if figure.endswith('iorate.png') ]
    access_hole_figures = [ os.path.join(sub_path, figure) for figure in os.listdir(analysis_result_path) if figure.endswith('hole.png') ]
    iorate_figures.sort()
    access_hole_figures.sort()
    figure_list = []
    for i in range(len(iorate_figures)):
        proc_info = (iorate_figures[i].partition("."))[0]
        r_index = string.rfind(proc_info, '_')
        proc_info = string.upper(proc_info[r_index+1:])
        iorate_figure = None
        hole_figure = None
        if os.path.getsize(os.path.join(iosig_data_path, iorate_figures[i])) > 0:
            iorate_figure = iorate_figures[i]
        if os.path.getsize(os.path.join(iosig_data_path, access_hole_figures[i])) > 0:
            hole_figure = access_hole_figures[i]
        figure_list.append((proc_info, iorate_figure, hole_figure))
    return render_template('iosig_details.html', trace_id=trace_id, figure_list=figure_list)



if __name__ == '__main__':
    app.debug = True
    app.run(host='0.0.0.0')

