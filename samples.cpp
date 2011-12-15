
/**
 * @example
 * This file shows basic use of VTApi.
 * 
 * @section CODE_SAPLES Some sample codes
 * @note After you create a new class, the position in the resultset of the query is set
 * to the value -1. Therefore it is necessary to use the method "next()" to move
 * the first row of the resultset.
 *
 * @subsection CODE_DATASET_VIEWALL Get names of datasets in the database
 * @code
 *    Dataset* dataset = this->newDataset();
 *    while (dataset->next() {
 *      cout << dataset->getName() << endl;
 *    }
 * @endcode
 * @note Use in other classes is very similar.
 *
 * @subsection CODE_TAGS_ADD Add a new tag into database (tag is some your class, not the class included in VTApi)
 * @code
 *    Dataset* dataset = this->newDataset();
 *    dataset->next();                                                   // Must be set some dataset
 *    KeyValues* tagskv = new KeyValues(*dataset, "tags");
 *    tagskv->insert = new Insert(*tagskv);
 *    tagskv->insert->setTable("tags");                                  // It's necessary to specify, into which table will be inserted
 *    tagskv->insert->keyString("real_name", "Some new tag");            // Set values of attributes
 *    tagskv->insert->keyString("notes", "Sample insertion of new tag");
 *    tagskv->insert->execute();                                         // It's necessary to execute complete composed query
 * @endcode
 * @note If you use a class from VTApi and you use it to insert new row to database,
 * use a method "add()" and execute by method "addExecute()" as follows.
 *
 * @subsection CODE_SEQUENCE_ADD Add a new sequence into database
 * @code
 *    Dataset* dataset = this->newDataset();
 *    dataset->next();
 *    Sequence* sequence = dataset->newSequence();
 *    sequence->add("Sequence_name", "Sequence_location");               // Set the name and the location of the new sequence
 *    sequence->addExecute();                                            // It's necessary to execute complete composed add (insert) command
 * @endcode
 * @note Use in other classes is very similar.
 *
 * @subsection CODE_TAG_WHEREQUERY Get realname of tag(s) using WHERE condition  (tag is some your class, not the class included in VTApi)
 * @code
 *    Dataset* dataset = this->newDataset();
 *    dataset->next();
 *    KeyValues* tagskv = new KeyValues(*dataset, "tags");
 *    tagskv->select = new Select(*tagskv);
 *    tagskv->select->from("tags", "*");                                 // It's necessary to specify, which columns from which table are interesting
 *    tagskv->select->whereString("real_name", "Some specific tag");     // Specify a condition
 *    tagskv->select->execute();                                         // It's necessary to execute complete composed query
 *    while (tagskv->next()) {
 *      cout << tagskv->getString("real_name") << endl;
 *    }
 * @endcode
 *
 * @subsection CODE_IMAGE_UPDATE Update some image to the new value
 * @code
 *    Dataset* dataset = this->newDataset();                              // You don't specify specific dataset, all datasets will be in resultset
 *    dataset->next();                                                    // Move to the first row of resultset
 *    Sequence* sequence = dataset->newSequence("some_sequence_name");    // You can specify in which sequence the image will be updated, only this specified sequence will be in resultset
 *    sequence->next();                                                   // Move to specified sequence
 *    image = sequence->newImage("some_image.jpg");                       // Specify which image will be updated
 *    image->next();                                                      // In case of update, the next() must be called (move to specified image)
 *    image->setString("imglocation", "updated_image.png");               // Do some edit
 * @endcode
 *
 * @subsection CODE_SELECT_SPECIAL Get number of sequences in the database (or select something other special than regular columns of table)
 * @code
 *    Dataset* dataset = this->newDataset();
 *    dataset->next();
 *    KeyValues* kv = new KeyValues(*dataset);                                                          // Create a KeyValues for select
 *    kv->select = new Select(*kv, "SELECT COUNT(*) FROM "+  dataset->getDataset() + ".sequences;");    // Specify full SQL select query
 *    kv->next();                                                                                       // Move to first row of resultset
 *    cout << "There are " << kv->getInt(0) << " sequences in this dataset." <<  endl;
 * @endcode
 *
 * @subsection CODE_GENERAL_QUERY Delete some image in time t=7 from sequence "testing".
 * @code
 *    Dataset* dataset = this->newDataset();
 *    dataset->next();
 *    Sequence* sequence = dataset->newSequence("testing");                                             // Specify sequence "testing"
 *    sequence->next();                                                                                 // Move to specified sequence "testing"
 *    query = new Query(*sequence, "DELETE FROM "+ dataset->getDataset() + ".intervals WHERE t1=7;");   // General query is made by class Query
 *    query->execute();
 * @endcode
 */
