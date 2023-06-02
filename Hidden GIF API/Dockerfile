# JKLMNOP! :)
FROM python:3.9-alpine

RUN apk add --no-cache gcc musl-dev

COPY mp2 /mp2
COPY mp7 /mp7

WORKDIR /mp7

RUN pip3 install -r requirements.txt

RUN apk add make
RUN make clean
RUN make

# EXPOSE 5000

CMD [ "python3", "-m" , "flask", "run", "--host=0.0.0.0", "--port=5000"]
 ENTRYPOINT ["/bin/sh"]