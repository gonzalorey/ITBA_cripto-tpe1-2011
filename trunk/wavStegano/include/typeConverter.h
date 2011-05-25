/* typeConverter.h */

#ifndef TYPECONVERTER_H_
#define TYPECONVERTER_H_

#include "fileContainer.h"
#include "dataHolder.h"

void typeConvertFCtoDH(fileContainer_t  *fileContainer, dataHolder_t *dataHolder);

void typeConvertDHtoFC(fileContainer_t  *fileContainer, dataHolder_t *dataHolder);

#endif /* TYPECONVERTER_H_ */
