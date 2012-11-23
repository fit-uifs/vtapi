/**
 * @example samples.cpp
 * This file shows basic use of VTApi.
 * 
 * @section CODE_SAPLES Some sample codes
 * @note To obtain first row of the resultset, you must use the method "next()"
 * after you construct Dataset, Sequence, Interval, Method and Process or after
 * you execute own query.
 *
 * @subsection CODE_GET_ALL_DATASET Get names of datasets in the database
 * @code
 *    VTApi* vtapi = new VTApi(argc, argv);
 *    Dataset* dataset = vtapi->newDataset();
 *    while (dataset->next()) {
 *      cout << dataset->getName() << endl;
 *    }
 * @endcode
 * @note If you would like to add new dataset, contact Petr Chmelar.
 *
 * @subsection CODE_GET_IMAGES_SEQUENCES Browse images by sequences
 * @code
 *    VTApi* vtapi = new VTApi(argc, argv);
 *    Dataset* dataset = vtapi->newDataset();
 *    if (! dataset->next()) error("No dataset");                        // You get first dataset, otherwise get some error
 *
 *    Sequence* sequence = dataset->newSequence("my_seq");               // Select sequence with name "my_seq"
 *    if (! sequence->next()) error("No my_seq");                        // Move to this sequence, otherwise get some error
 *    Image* image = sequence->newImage();                               // Select all images in current sequence
 *    while (image->next()) cout << image->getString("imglocation");     // For each image write image location to the ouput
 * @endcode
 *
 * @subsection CODE_ADD_SEQUENCE Add a new sequence into database
 * @code
 *    VTApi* vtapi = new VTApi(argc, argv);
 *    Dataset* dataset = vtapi->newDataset();
 *    if (! dataset->next()) error("No dataset");
 *
 *    Sequence* sequence = dataset->newSequence();
 *    sequence->add("Sequence_name", "Sequence_location");               // Set the name and the location of the new sequence (basic data of sequence)
 *    sequence->addInt("seqnum", 7);                                     // You can add some other information, for example sequence number
 *    sequence->addExecute();                                            // It's necessary to execute complete composed add (insert) command
 * @endcode
 *
 * @subsection CODE_GET_AND_UPDATE_ARRAY_OF_INTERVAL Get and update an array of interval (image)
 * @code
 *    VTApi* vtapi = new VTApi(argc, argv);                              // Lines below are shown for the complete sample
 *    Dataset* dataset = vtapi->newDataset();
 *    if (! dataset->next()) error("No dataset")
 *    Sequence* sequence = dataset->newSequence("some_seq");
 *    if (! sequence->next()) error("No some_seq");
 *
 *    Image* image = sequence->newImage("some_image");
 *    if (! image->next()) error("No some_image");
 *
 *    int size = 0;                                                      // You need some variable which gives the number of array elements.
 *    float* array = image->getFloatA("test", size);                     // You get float array from attribute test for current image and variable size is also updated
 *    // Some code, where you change some values in array
 *    image->setFloatA(array, size);                                     // You set new array value of attribute test for current image. The array has <size> of elements.
 *    image->setExecute();                                               // You apply changes using setExecute.
 * @endcode
 * @note It's recommended to consult this use.
 *
 * @subsection CODE_TAG_WHEREQUERY Work with arbitrary table - get realname of tags using WHERE condition
 * @code
 *    VTApi* vtapi = new VTApi(argc, argv);
 *    Dataset* dataset = vtapi->newDataset();
 *    if (! dataset->next()) error("No dataset")
 *
 *    KeyValues* tagskv = new KeyValues(*dataset, "tags");               // Construct KeyValues above the table tags in current dataset.
 *    tagskv->select = new Select(*tagskv);                              // Create a select query above the table tags.
 *    tagskv->select->whereString("real_name", "some value");            // Specify a condition for query above the table tags
 *    while (tagskv->next()) {                                           // You execute query using method "next()".
 *      // ... some code of processing
 *    }
 * @endcode
 * @note It's recommended to consult this use.
 *
 * @subsection CODE_GENERAL_QUERY Delete tags in time t=7 from table "tags"
 * Deleting is solved as general query.
 * @code
 *    VTApi* vtapi = new VTApi(argc, argv);
 *    Dataset* dataset = vtapi->newDataset();
 *    if (! dataset->next()) error("No dataset")
 *    KeyValues* tagskv = new KeyValues(*dataset, "tags");
 * 
 *    tagskv->update = new Query(*tagskv, "DELETE FROM "+ tagskv->getDataset() + ".tags WHERE tagid=7");   // General query is made by class Query
 *    tagskv->update->execute();                                         // You execute query using method "execute()".
 * @endcode
 * @note It's recommended to consult this use.
 *
 * @subsection CODE_ADD_PROCESS Add new process
 * @warning It's necessary to consult this request.
 *
 * @subsection CODE_ADD_DATASET_OR_METHOD Add new dataset, method
 * @warning Only Petr Chmelar executes this request.
 * 
 */
