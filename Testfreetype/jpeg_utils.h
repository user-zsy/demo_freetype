/**
 * @file   jpeg_utils.h
 * @author Late Lee 
 * @date   2012-7-1 12:47:56
 * @brief  
 *         ʹ��libjpeg���дJPEGͼƬ�ļ���
 *
 * @note   ��ѹ������������ΪRGB
 *
 */

#ifndef _JPEG_BMP_H
#define _JPEG_BMP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ����libjpeg��ȡJPEGͼƬ�ļ�
 * 
 * @param[IN]   jpg_fle    JPEGͼƬ�ļ�����
 * 
 * @param[OUT]  rgb_buffer JPEGͼƬRGB����
 * @param[OUT]  size       RGB���ݴ�С
 * @param[OUT]  width      ͼƬ��
 * @param[OUT]  height     ͼƬ��
 *
 * @return 
 *         0���ɹ�
 *         -1����ȡ�ļ�ʧ�ܣ��������ڴ�ʧ��
 * @note
 *        1��rgb_bufferΪ����ָ�룬�ڴ��ɸú������䣬��Ҫ�����ͷ�
*         2��rgb_buffer����������ΪRGB������Ҫ�����BMP����Ҫ���е���ΪBGR
 */
int read_jpg_file(const char* jpg_file, unsigned char** rgb_buffer, int* size, int* width, int* height);

/**
 * ����libjpeg��ȡJPEGͼƬ�ڴ�
 * 
 * @param[IN]   jpg_buffer JPEGͼƬ����
 * @param[IN]   jpg_size   JPEGͼƬ���ݴ�С
 * @param[OUT]  rgb_buffer RGB��ʽ����
 * @param[OUT]  size       RGB���ݴ�С
 * @param[OUT]  width      ͼƬ��
 * @param[OUT]  height     ͼƬ��
 *
 * @return 
 *         0���ɹ�
 *         -1����ȡ�ļ�ʧ�ܣ��������ڴ�ʧ��
 * @note
 *         1��rgb_bufferΪ����ָ�룬�ڴ��ɸú������䣬��Ҫ�����ͷ�
 *         2��rgb_buffer����������ΪRGB������Ҫ�����BMP����Ҫ���е���ΪBGR
 */
int read_jpg_buffer(unsigned char* jpg_buffer, int jpg_size, unsigned char** rgb_buffer, 
                    int* size, int* width, int* height);

/**
 * ����libjpegת��ΪJPEGͼƬ������
 * 
 * @param[IN]  jpg_fle    JPEGͼƬ�ļ�����
 * 
 * @param[IN]  rgb_buffer JPEGͼƬRGB����
 * @param[IN]  width      ͼƬ��
 * @param[IN]  height     ͼƬ��
 * @param[IN]  quality    ͼƬ����
 *
 * @return 
 *         0���ɹ�
 *         -1�����ļ�ʧ��
 * @note
 *         1��ͼƬ����δʵ�ʲ���
 *         2��rgb_buffer��������ΪRGB
 */
int write_jpg_file(const char* jpg_file, unsigned char* rgb_buffer, int width, int height, int quality);

#ifdef __cplusplus
};
#endif

#endif /* _JPEG_BMP_H */