import qbs
import qbs.Probes as Probes

Project
{
    qbsSearchPaths: "qbs"
    references: [
            "core/core.qbs",
            "src/src.qbs"
    ]
}
