#include "TiffImporter.h"

#include <QDebug>

#include "gdal/gdal_alg.h"
#include "gdal/ogrsf_frmts.h"

#include "utils/geometry/Coordinate.h"

TiffImporter::TiffImporter(QString path , QString file_name , QObject *parent) : GSSImporterExporter( path , file_name , "tiff" , parent ){

}

/**********************************************************************
 GETTERS
**********************************************************************/

/**
   WARNING Caller gets ownership of the result
 * @brief TiffImporter::getValues
 * @return
 */
GWSGrid TiffImporter::getValues(){

    GWSGrid grid;
    double left, right, cell_width, top, bottom, cell_height, min, max = 0;

    GDALDataset *dataset;
    GDALAllRegister();

    QByteArray ba = this->directory_path.toLatin1();
    const char *c_str = ba.data();

    dataset = (GDALDataset *) GDALOpen( c_str , GA_ReadOnly );
    if( dataset == NULL ) {
        qWarning() << QString("%1 %2").arg( "GDAL cant find file" ).arg( directory_path );
        return grid;
    }

    qDebug() << QString("Driver: %1 %2").arg( dataset->GetDriver()->GetDescription() ).arg( dataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
    qDebug() << QString("Size: %1 %2").arg( dataset->GetRasterXSize() ).arg( dataset->GetRasterYSize() + dataset->GetRasterCount() );

    if( dataset->GetProjectionRef() != NULL ) {
        qDebug() << QString("Projection %1").arg( dataset->GetProjectionRef() );
    }

    GDALRasterBand  *band;
    int             nBlockXSize, nBlockYSize;
    int             bGotMin, bGotMax;
    double          adfMinMax[2];
    double          adfGeoTransform[6];

    if( dataset->GetGeoTransform( adfGeoTransform ) == CE_None ){
        qDebug() << QString("Origin %1 %2").arg( adfGeoTransform[0] ).arg( adfGeoTransform[3] );
        qDebug() << QString("Pixel Size %1 %2").arg( adfGeoTransform[1] ).arg( adfGeoTransform[5] );
    }

    // Left, top, cell_width and cell_height
    left = adfGeoTransform[0];
    top = adfGeoTransform[3];
    cell_width = adfGeoTransform[1];
    cell_height = adfGeoTransform[5];

    // Iterate all bands
    for(int i = 0 ; i < dataset->GetRasterCount() ; i++){

        band = dataset->GetRasterBand( i+1 ); // Starts in 1
        band->GetBlockSize( &nBlockXSize, &nBlockYSize );

        qDebug() << QString("Block %1 %2, Type %3, ColorInterp %4").arg( nBlockXSize ).arg( nBlockYSize ).arg( GDALGetDataTypeName(band->GetRasterDataType()) ).arg( GDALGetColorInterpretationName( band->GetColorInterpretation()) );

        adfMinMax[0] = band->GetMinimum( &bGotMin );
        adfMinMax[1] = band->GetMaximum( &bGotMax );

        if( ! (bGotMin && bGotMax) ){
            GDALComputeRasterMinMax((GDALRasterBandH)band, TRUE, adfMinMax);
        }

        qDebug() << QString("Min value %1" ).arg( adfMinMax[0] );
        qDebug() << QString("Max value %1" ).arg( adfMinMax[1] );
        min = adfMinMax[0];
        max = adfMinMax[1];

        if( band->GetOverviewCount() > 0 ){
            qDebug() << QString("Band has %1 overviews" ).arg( band->GetOverviewCount() );
        }

        if( band->GetColorTable() != NULL ){
            qDebug() << QString("Band has a color table with %1 entries" ).arg( band->GetColorTable()->GetColorEntryCount() );
        }

        int x_size = band->GetXSize();
        int y_size = band->GetYSize();
        right = left + ( cell_width * x_size );
        bottom = top + ( cell_height * y_size );

        grid = GWSGrid( left , right , top , bottom , x_size , y_size , min , max );

        // Make malloc buffer
        float* pafScanline = (float *) CPLMalloc(sizeof(float)*x_size*y_size);

        // Read band into buffer
        band->RasterIO( GF_Read, 0, 0, x_size, y_size, pafScanline, x_size, y_size, GDT_Float32, 0, 0 );

        // Iterate buffer
        int pos = 0;
        for (int y = 0; y < y_size; y++){
            for (int x = 0; x < x_size; x++){

                double value = pafScanline[pos];

                double cell_left = left + ( cell_width * x );
                double cell_top = top + ( cell_height * y );
                double cell_right = left + ( cell_width * (x+1) );
                double cell_bottom = top + ( cell_height * (y+1) );

                grid.setValue( GWSCoordinate( ( cell_left + cell_right ) / 2 , ( cell_top + cell_bottom ) / 2 ) , value );

                pos++;
            }
        }

        // Free buffer
        CPLFree(pafScanline);
    }

    GDALClose( dataset );

    qInfo() << QString("Created %1 x %2 elevation grid").arg( grid.getXSize() ).arg( grid.getYSize() );

    return grid;
}



