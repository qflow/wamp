import qbs
import qbs.Probes as Probes

QmlPlugin
{
    name: "wamp"
    cpp.cxxLanguageVersion: "c++11"
    files: [
        "client/call.cpp",
        "client/call.h",
        "gssapiauthenticator.cpp",
        "gssapiauthenticator.h",
        "gssapiuser.cpp",
        "gssapiuser.h",
        "helper.cpp",
        "helper.h",
        "router/authenticator.cpp",
        "router/authenticator.h",
        "router/authorizer.cpp",
        "router/authorizer.h",
        "router/defaultauthorizer.cpp",
        "router/defaultauthorizer.h",
        "router/realm.cpp",
        "router/realm.h",
        "router/realm_p.h",
        "router/role.cpp",
        "router/role.h",
        "sid.cpp",
        "sid.h",
        "user.cpp",
        "user.h",
        "user_p.h",
        "wamp.h",
        "wamp_global.h",
        "wamp_symbols.h",
        "wampattached.h",
        "wampbase.cpp",
        "wampbase.h",
        "client/wampconnection.h",
        "router/wampcraauthenticator.cpp",
        "router/wampcraauthenticator.h",
        "wampcrauser.cpp",
        "wampcrauser.h",
        "wamperror.h",
        "wampinvocation.h",
        "router/wamprouter.cpp",
        "router/wamprouter.h",
        "router/wamprouter_p.h",
        "router/wamproutersession.cpp",
        "router/wamproutersession.h",
        "router/wamprouterworker.cpp",
        "router/wamprouterworker.h",
        "client/wampworker.h",
        "credentialstore.h",
        "client/wampconnection_p.h",
        "client/registration_p.h",
        "client/subscription_p.h",
        "wamp.cpp",
        "credentialstore.cpp",
        "wampattached.cpp",
        "client/wampconnection.cpp",
        "client/wampworker.cpp",
        "wamperror.cpp",
        "treemodel.cpp",
        "treemodel.h",
        "wampmessageserializer.cpp",
        "wampmessageserializer.h",
        "router/wamproutersession_p.h",
    ]

    pluginNamespace: "QFlow.Wamp"
    pluginRootPath: "bin/plugins"
    cpp.includePaths: [".", "router", "client"]

    cpp.dynamicLibraries:
    {
        var libs = [];
        if(qbs.targetOS.contains("windows"))
        {
            libs.push("Advapi32", "Secur32");
        }

        return libs;
    }

    Group
    {
        name: "libraries"
        qbs.install: true
        qbs.installDir: "bin"
        files: {
        }
        fileTags: "libraries"
    }
    Depends{
        name: "libsecret"
        condition: qbs.targetOS.contains("linux")
    }
    Depends{name: "msgpack"}
    Depends{name: "websockets"}


    Depends{name: "Qt"; submodules: ["network"]}
    Depends{name: "core"}
}
