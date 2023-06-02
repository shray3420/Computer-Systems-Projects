import pytest
from dotenv import load_dotenv
import json, io, os
import requests
import subprocess, time
import signal

load_dotenv()

DOCKER_build = "docker build -t mp7-docker -f Dockerfile ../"
DOCKER_run = "docker run -d -it -p 5000:5000 mp7-docker"
DOCKER_list = "docker ps -a -q --filter ancestor=mp7-docker"
DOCKER_stop = "docker stop"
base_url = "http://127.0.0.1:5000"

@pytest.fixture(scope="session", autouse=True)
def pytest_sessionstart():
    print(DOCKER_build)
    proc_build = subprocess.Popen(DOCKER_build.split())
    proc_build.wait()
    assert(proc_build.returncode == 0)

    print(DOCKER_run)
    proc_run = subprocess.Popen(DOCKER_run.split())
    time.sleep(5)
    proc_run.wait()
    assert(proc_run.returncode == 0)

    yield

    print(DOCKER_list)
    proc_ps = subprocess.Popen(DOCKER_list.split(), stdout=subprocess.PIPE, text=True)
    container_id, _ = proc_ps.communicate()
    assert(proc_ps.returncode == 0)

    proc_kill = subprocess.Popen(f'{DOCKER_stop} {container_id}'.split())
    proc_kill.wait()
    assert(proc_kill.returncode == 0)


def test_docker_all():
    data = {'png': open('sample/no-uiuc-chunk.png', 'rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 415)

    data = {'png': open('sample/waf.gif', 'rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 422)

    data = {'png': open('sample/waf.png','rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 200)

    data = {'png': open('sample/natalia.png','rb')}
    response = requests.post(base_url+'/extract', files=data)
    assert(response.status_code == 200)
