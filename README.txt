/**
 * @mainpage
 *
 * VTApi (Video Terror Application programming interface and methodology),
 * a data(base) interface used for processing and efficient management, indexing,
 * retrieval and analysis of image and video data and related metadata,
 * which was created to unify and accelerate the intelligent vision
 * applications development.
 *
 * VTApi 1.5 is based on PostgreSQL database, PostGIS and OpenCV created within
 * the Ministry of interior VideoTerror (VT) research project. The main objective
 * of the VT project is to define, explore and create a prototype of a system
 * warehousing image and video accomplished with computer
 * vision and analytics based on a computer cluster. The basic requirements
 * include image and video feature extraction, storage and indexing to enable
 * (content-based) retrieval, summarization and characterization together with
 * video analytics in the meaning of object detection and recognition
 * in an interactive and iterative process.
 *
 * @section HOMEPAGE VTApi download and install
 *
 * Wiki https://gitorious.org/vtapi/pages/Install
 * contains basic prerequisities, compilation and installation manual.
 *
 * Source codes in various versions can be found at https://gitorious.org/vtapi .
 *
 * Download the actual version at https://gitorious.org/vtapi/vtapi/archive-tarball/master .
 *
 * Windows install v1.0 info is located at README at
 * http://vidte.fit.vutbr.cz/dist/VTApi_Windows.zip .
 *
 * Brno University of Technology information
 * http://vidte.fit.vutbr.cz/vtapi.html
 * contains technical documentation (in Czech), recommended for beginners.
 *
 * @subsection START Getting started
 *
 * @subsubsection LOGICAL Logical model of VTApi
 * @image html minimal_logical_data_model_small.png "Logical model of VTApi"
 * @image latex minimal_logical_data_model_small.png "Logical model of VTApi"
 * @image rtf minimal_logical_data_model_small.png "Logical model of VTApi"
 *
 * - Dataset is a named set of (multimedia) data along with metadata
 *         (descriptive data). Datasets can be organized hierarchically, i.e., one
 *         may be based on several others. Each dataset contains sequences.
 * - Sequence is a named ordered set of frames referred to as  Video
 *         or  Images. The ordering of frames
 *         in video is time-based. There may be their intervals defined for a sequence.
 * - Interval is any subsequence of  Videoor  Images
 *         whose elements share the same metadata. For example, it can be a video
 *         shot or any sequence of  frames containing the monitored object in
 *         the video or scene. Metadata of an interval are created by a process.
 * - Method defines
 *         the custom algorithm and the structure of metadata consumed and produced by
 *         a  Processes - a running instance of this type.
 * - Process (task or operation) is a named run of Method.
 *        Process defines (inserts and modifies) data according to its
 *	  inputs (created by other processes,
 *        media data) and it represents all activities of the proposed framework.
 *        Implementation of a specific method is generally not included in the API,
 *        it is created by developers using the API.
 * - @b Tag is an indexing term representing an ontology class (in hierarchy).
 *         Tags are assigned to the multimedia data as description or annotation of a scene,
 *         object or action.
 * - @b Selection is a subset of logically related metadata,
 *         appropriately chosen, so that operations (processes) are effective and
 *         allow the natural chaining of processes (input, output of a process or media data).
 *         Common examples of selections are  Intervaland  Tag. This concept is
 *         related to the effective implementation and access to the metadata in the database.
 *
 * @subsubsection KEYVALUES KeyValues
 * The KeyValue is a basic mechanism of organization metadata in VTApi. It is
 * data structure, which allows you to store metadata into pair <key, value>.
 *
 * Most classes inherit from KeyValues that provides the basic
 * operations needed to manage key-value pairs (associative array), on which
 * the VTApi model is based. KeyValues class is the crucial to ensure
 * the functionality and generality of the API by the main function next(), which performs
 * most operations of the API, except constructors. It performs the database queries similarly to JDBC.
 * It also allows to change the values of object's variables and
 * commits the values changed using setters and
 * it inserts values using our adders methods.
 * Moreover, it uses the lazy approach, hiding the functionality necessary, doing
 * it efficiently when needed by using caches and batches if possible.
 *
 * The VTApi is strongly typed, the following description uses notation of X
 * referring to any data type implemented (integers, floating points, strings,
 * 4D geometry points, lines and polygons and their structures, vectors, arrays
 * and (OpenCV) matrices).
 * For instance, getX(k) or setX(k,v) operates key k and its value v of type X.
 *
 * The entry point to the application is the VTApi class based on the config
 * file and command-line arguments.
 *
 * All other classes, denoted as C, are derived from KeyValues, and they inherit
 * its operations and attributes:
 * - Commons class provides a very basic functions such as loading
 *          configuration file and command line parameters (using GNU Gengetopt),
 *         it provides a connection to the database (PostgreSQL), a data storage
 *         (remote file system) and it uniformly manages error reports and other statements (log).
 *         Commons is a shared object, usually created by the VTApi class.
 * - Select class is used to construct queries that after the first
 *         call of the function next() retrieve information from the database.
 *         There are special functions to simplify the construction of queries,
 *         so that it is basically mostly satisfactory to use the constructor for most of them. Other
 *         functions simplify the work with selections, keys and their values to filter queries,
 *         use functions and indexes.
 * - Insert class provides insertion of defined data where possible
 *           using the function addX(k,v). There are 2 general ways of inserting --
 *         immediate (addExecute()) or batch (implicitly) by calling next().
 * - Update class similarly allows the modification of values of the
 *         current element using the the typed family of functions setX(k,v).
 *
 * Classes derived from KeyValues contain only a minimum of functionality programmed,
 * so it is easy to create a new derived class C if needed.
 * Above all, they take care of consistency of data and provide simple accelerator functions.
 * For instance, getName() for their identifier or getLocation() for the physical data location
 * (e.g., a dataset or a directory with pictures) and newC() methods, that show the
 * developer the natural flow of the program (represented as aggregations in figure above.
 * For example, invoking newSequence() method of the Dataset class object creates
 * a new object of class Sequence, with all necessary parameters, in which we can call the next().
 *
 * See <a href="./examples.html">Examples</a> for more details on configuration and code samples.
 *
 * @copyright Brno University of Technology &copy; 2011 &ndash; 2012
 *
 * VTApi is distributed under BUT OPEN SOURCE LICENCE (Version 1).
 * This licence agreement provides in essentials the same extent of rights as the terms of GNU GPL version 2 and Software fulfils the requirements of the Open Source software.
 *
 * @authors
 * Petr Chmelar, chmelarp (at) fit.vutbr.cz
 * Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * Tomas Volf, ivolf (at) fit.vutbr.cz
 *
 * @example vtapi.conf
 * This file shows an example configuration file for VTApi.
 */
 

File structure:
 build
 dist
 doc
 include
 nbproject
 postgres
 pyi
 src
 Makefile
 README.txt
 vtapi.conf
 vtcli.cpp


