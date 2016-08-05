# Introduction
[Router](src/client/README.md)
# Get sources
    git clone git@github.com:qflow/wamp.git
    cd wamp
    git submodule update --init --remote
    git submodule foreach git pull origin master
    git submodule foreach git checkout master

# Build
    Use CMake

    open main CMakeLists.txt in QtCreator
    
# Client
## QML
```qml
import QFlow.Core 1.0
import QFlow.Networking 1.0
import QFlow.Wamp 1.0

...
QtObject //example object that will be registered as "object"
    {
        id: root
        property int testInt: 5 //will be registered as "object.testInt"
        function testFunc1() //will be registerd as "object.testFunc"
        {

        }
        property list<QtObject> objList: [ //will be registered as "object.ojList"
            QtObject{property int p: 4}, //will be registered as "object.objList.0"
            QtObject{property string s: "hello"}] ////will be registered as "object.objList.1"
        property QtObject child: QtObject{ //will be registered as "object.child"
            id: child
            function substract(a,b) { //will be registered as "object.child.substract"
                    return a-b;
                }

        }
    }
    
WampConnection
    {
        id: wampConnection
        url: "ws://localhost:8080"
        realm: "realm1"
        user: WampCraUser
        {
            name: "name"
            secret: "secret"
        }
        onConnected:
        {
            wampConnection.registerProcedure("test.add", wampConnection.add); //register procedure
            wampConnection.subscribe("com.myapp.hello", function(param1){ //subscribe to event
                console.log(param1); //event received
            });
            wampConnection.registerObject("object", root); //register object with properties, functions, recursively registering children
            
            var future = wampConnection.call("test.add", [1,2]); //call remote procedure
            future.then(function(){
                console.log(future.result()); //display result
            });
            wampConnection.publish("com.myapp.hello", ["hello"]); //publish event
        }
        function add(a,b)
        {
            return a + b;
        }
        onError:
        {
            console.log(error);
        }
    }

```

## C++
```cpp
include <wampconnection.h>
using namespace QFlow;

...
//initialize wamp connection
WampConnection* con = new WampConnection();
con->setRealm("realm1");
con->setUser(new WampCraUser("name", "secret", con));
con->setUrl(QUrl("ws://localhost:8080"));
con->connect();

//use connection

//register lambda
con->registerProcedure("test.add", [](int a, int b){
        return a + b;
});
    
//register QObject
con->registerObject("object", obj);

//register method of object 'obj'
con->registerMethod("object.testFunc1", obj, "testFunc1()");

//register integer property 'testInt'. The property getter will be at "object.testInt" and the setter at "object.testInt.set"
con->registerProperty("object.testInt", obj, "testInt")

//subscribe and specify lambda callback
con->subscribe("com.myapp.hello", [](QString param1){
    qDebug() << param1; //event captured
});

//call remote procedure
Future f = con->call("test.add", {1, 2});
f.then([this](const Future& future){
    qDebug() << future.result(); //result received
});

// publish event
con->publish("com.myapp.hello", {"hello"});


```
