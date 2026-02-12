package com.example.logic_server.global.config;

import org.apache.kafka.clients.admin.NewTopic;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.kafka.annotation.EnableKafka;
import org.springframework.kafka.config.TopicBuilder;
import org.springframework.kafka.support.converter.JacksonJsonMessageConverter;
import org.springframework.kafka.support.converter.RecordMessageConverter;

@Configuration
@EnableKafka
public class KafkaConfig {
    @Bean
    public RecordMessageConverter recordMessageConverter() {
        return new JacksonJsonMessageConverter();
    }

    @Bean
    public NewTopic characterTopic() {
        return TopicBuilder.name("CHARACTER")
                .partitions(1)
                .replicas(1)
                .build();
    }
}
